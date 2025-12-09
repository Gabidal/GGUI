/**
 * @file time2.cpp
 * @brief Compare two Callgrind outputs and emit a filtered Callgrind file.
 *
 * Build target: time2 (wired via bin/meson.build)
 *
 * Usage:
 *   ./time2 run1.out run2.out threshold
 *
 * Behavior:
 * - Parses per-function instruction counts by aggregating cost lines under each `fn=` block.
 * - Computes growth ratio = count2 / max(1, count1).
 * - Emits a Callgrind-compatible file `filtered.out` containing only functions with ratio >= threshold
 *   along with their immediate callers and callees (to keep context in viewers like KCachegrind).
 */

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using FnCounts = std::unordered_map<std::string, long long>;
using FnRatios = std::unordered_map<std::string, double>;
using StringVec = std::vector<std::string>;

/**
 * @brief Trim ASCII whitespace from both ends of the string.
 */
static inline std::string trimString(const std::string &s) {
    size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

/**
 * @brief Check if string starts with a given prefix.
 */
static inline bool startsWith(const std::string &s, const char *prefix) {
    const size_t n = std::strlen(prefix);
    return s.size() >= n && std::memcmp(s.data(), prefix, n) == 0;
}

/**
 * @brief Parsed representation of a Callgrind file (subset relevant to this tool).
 *
 * This tool uses a lightweight parser. It tracks the current function with lines
 * starting with `fn=<name>` and aggregates numeric costs from lines that begin
 * with a number. It does not distinguish inclusive/exclusive costs – the goal is
 * to approximate growth signals for quick comparisons.
 */
struct ParsedFile {
    FnCounts fnCounts;                 ///< Aggregated per function.
    StringVec header;                  ///< Header/meta lines to copy (from file2 if available).
    // We also retain raw blocks per function from file2 for re-emission.
    // Each block is a vector of lines starting at fl= or fn= up to before next fn=/fl=/summary.
    std::unordered_map<std::string, StringVec> fnBlocks; // only filled for second file
    // Call graph adjacency captured from cfn= lines inside each function block.
    std::unordered_map<std::string, StringVec> callees;
};

/**
 * @brief Parse the last integer-like token from a cost line.
 *
 * Cost lines can be either "ln cost" or just "cost"; this extracts the last integral
 * token and converts it to 64-bit. Non-numeric lines return 0.
 */
static long long parseCostvalue(const std::string &line) {
    std::istringstream iss(line);
    std::string tok, last;
    while (iss >> tok) last = tok;
    if (last.empty()) return 0;
    // Remove commas to accept formatted numbers
    last.erase(std::remove(last.begin(), last.end(), ','), last.end());
    char *endp = nullptr;
    errno = 0;
    const long long v = std::strtoll(last.c_str(), &endp, 10);
    if (errno != 0 || endp == last.c_str()) return 0;
    return v;
}

/**
 * @brief Parse a Callgrind file, optionally capturing full blocks for re-emission.
 *
 * @param path           Path to the input file.
 * @param captureBlocks  If true, store each function block's lines and cfn= edges.
 * @return ParsedFile    Aggregated function costs and optionally blocks/edges.
 */
static ParsedFile parseCallgrind(const std::string &path, bool captureBlocks) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    ParsedFile pf;
    std::string line;
    std::string currentFn;
    StringVec currentBlock;
    bool inHeader = true;

    while (std::getline(in, line)) {
        // Preserve original line endings in output; we'll write with \n, KCachegrind accepts LF.
        const std::string t = trimString(line);

        if (inHeader) {
            // Header continues until the first content marker ("fl=" or "fn=" or "events:")
            if (startsWith(t, "fl=") || startsWith(t, "fn=")) {
                inHeader = false;
            } else if (startsWith(t, "events:")) {
                // still header but continue
            }
        }

        if (inHeader) {
            pf.header.push_back(line);
            continue;
        }

        // Detect new function block
        if (startsWith(t, "fn=")) {
            // Flush previous block
            if (captureBlocks && !currentFn.empty() && !currentBlock.empty()) {
                pf.fnBlocks[currentFn] = currentBlock;
            }
            currentFn = trimString(t.substr(3)); // function name only
            currentBlock.clear();
            if (captureBlocks) currentBlock.push_back(line); // keep original 'fn=' line formatting
            continue;
        }

        // File marker can belong to block; include it
        if (startsWith(t, "fl=")) {
            if (captureBlocks && !currentFn.empty()) currentBlock.push_back(line);
            continue;
        }

        // Summary and meta lines mark end of blocks
        if (startsWith(t, "summary:") || startsWith(t, "events:") || startsWith(t, "event:")) {
            if (captureBlocks && !currentFn.empty() && !currentBlock.empty()) {
                pf.fnBlocks[currentFn] = currentBlock;
                currentBlock.clear();
            }
            // Keep meta lines after header if we want; but per spec we copy header from second file only
            continue;
        }

        // Inside a function: count numeric-leading cost lines only
        if (!currentFn.empty()) {
            // Track call edges (cfn=<callee>)
            if (startsWith(t, "cfn=")) {
                const std::string callee = trimString(t.substr(4));
                if (!callee.empty()) pf.callees[currentFn].push_back(callee);
            }
            if (captureBlocks) currentBlock.push_back(line);
            if (!t.empty() && std::isdigit(static_cast<unsigned char>(t[0]))) {
                // Try to parse a cost at end of line
                const long long val = parseCostvalue(t);
                if (val > 0) {
                    pf.fnCounts[currentFn] += val;
                }
            }
        } else {
            // Lines before first fn= but after header: ignore
        }
    }

    // Flush last block
    if (captureBlocks && !currentFn.empty() && !currentBlock.empty()) {
        pf.fnBlocks[currentFn] = currentBlock;
    }

    return pf;
}

/**
 * @brief Compute growth ratios for functions from run2 relative to run1.
 *
 * @param fn1 Baseline counts from run1.
 * @param fn2 Comparison counts from run2.
 * @return FnRatios Map of function -> ratio (0.0 if missing in run2; >= 1.0 indicates growth).
 */
static FnRatios computeGrowth(const FnCounts &fn1, const FnCounts &fn2) {
    FnRatios out;
    // Consider union of keys from both maps
    out.reserve(fn1.size() + fn2.size());
    for (const auto &kv : fn2) {
        const std::string &fn = kv.first;
        long long c2 = kv.second;
        long long c1 = 0;
        auto it1 = fn1.find(fn);
        if (it1 != fn1.end()) c1 = it1->second;
        const double ratio = (c2 > 0) ? static_cast<double>(c2) / static_cast<double>(std::max<long long>(1, c1)) : 0.0;
        out[fn] = ratio;
    }
    // Include functions present only in run1 (ratio 0)
    for (const auto &kv : fn1) {
        if (out.find(kv.first) == out.end()) out[kv.first] = 0.0;
    }
    return out;
}

/**
 * @brief Filter functions by growth threshold and write a Callgrind-compatible output.
 *
 * Selected functions are those whose ratio >= threshold. We also include their immediate
 * callers and callees to preserve some context in the call graph.
 */
static void filterAndWrite(const ParsedFile &pf2, const FnCounts &fn1, const FnCounts &fn2, double threshold, const std::string &outPath) {
    std::ofstream out(outPath);
    if (!out) {
        throw std::runtime_error("Failed to open output file: " + outPath);
    }

    // Write header from file2; if missing, synthesize minimal
    if (!pf2.header.empty()) {
        for (const auto &h : pf2.header) out << h << '\n';
    } else {
        out << "creator: time2.cpp" << '\n';
        out << "events: Ir" << '\n';
    }

    // Precompute growth ratios
    const FnRatios ratios = computeGrowth(fn1, fn2);

    // Select interesting functions
    std::unordered_map<std::string, bool> selected;
    selected.reserve(ratios.size());
    for (const auto &rk : ratios) {
        if (rk.second >= threshold) selected[rk.first] = true;
    }
    // Expand with immediate callees
    for (const auto &sk : selected) {
        auto it = pf2.callees.find(sk.first);
        if (it != pf2.callees.end()) {
            for (const auto &callee : it->second) selected[callee] = true;
        }
    }
    // Build reverse edges (callers) and expand with immediate callers
    std::unordered_map<std::string, StringVec> callers;
    for (const auto &kv : pf2.callees) {
        const std::string &caller = kv.first;
        for (const auto &callee : kv.second) callers[callee].push_back(caller);
    }
    for (const auto &sk : selected) {
        auto it = callers.find(sk.first);
        if (it != callers.end()) {
            for (const auto &caller : it->second) selected[caller] = true;
        }
    }

    // Emit blocks only for selected functions, preserving stored block formatting
    for (const auto &kv : pf2.fnBlocks) {
        const std::string &fn = kv.first;
        if (selected.find(fn) != selected.end()) {
            for (const auto &ln : kv.second) out << ln << '\n';
        }
    }
}

int main(int argc, char **argv) {
    // Basic CLI contract
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " run1.out run2.out threshold" << std::endl;
        return 1;
    }
    const std::string f1 = argv[1];
    const std::string f2 = argv[2];
    const std::string thrS = argv[3];
    char *endp = nullptr;
    errno = 0;
    double threshold = std::strtod(thrS.c_str(), &endp);
    if (errno != 0 || endp == thrS.c_str()) {
        std::cerr << "Invalid threshold: " << thrS << std::endl;
        return 1;
    }
    if (threshold < 0.0) threshold = 0.0;

    try {
        ParsedFile pf1 = parseCallgrind(f1, /*captureBlocks=*/false);
        ParsedFile pf2 = parseCallgrind(f2, /*captureBlocks=*/true);

        // Filtering and writing
        filterAndWrite(pf2, pf1.fnCounts, pf2.fnCounts, threshold, "filtered.out");

        std::cout << "Filtered Callgrind file written to filtered.out" << std::endl;
    } catch (const std::exception &ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
