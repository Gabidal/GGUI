#include "ecma.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {
            static table::configuration::manager pageState;

            namespace sequence {
                std::string prefix::toString() const {
                    // only primary set
                    if (std::holds_alternative<table::C0>(function)) {
                        return table::toString(std::get<table::C0>(function));
                    } else {
                        return table::toString(table::C0::ESC) + table::toString(std::get<table::C1>(function));
                    }
                }

                std::string toString(std::variant<table::finalWithoutIntermediate, table::finalWithIntermediate> controlStringFinalByte) {
                    std::string result = "";

                    if (std::holds_alternative<table::finalWithoutIntermediate>(controlStringFinalByte)) {
                        result += static_cast<char>(std::get<table::finalWithoutIntermediate>(controlStringFinalByte));
                    } else {
                        result += static_cast<char>(std::get<table::finalWithIntermediate>(controlStringFinalByte));
                    }

                    return result;
                }

                /**
                 * Converts the control string to its string representation.
                 * 
                 * Control strings follow the format:
                 *   - 7-bit: ESC (01/11) + opening delimiter (C1) + characters + ESC + ST (05/12)
                 *   - 8-bit: 8-bit opening delimiter + characters + 8-bit ST (09/12)
                 * 
                 * The opening delimiter can be one of: APC, DCS, OSC, PM, or SOS from the C1 table.
                 */
                std::string string::toString() const {
                    std::string result = prefix::toString();

                    // Output all character bytes
                    for (uint8_t characterByte : characters) {
                        result += static_cast<char>(characterByte);
                    }

                    result += terminator.toString();

                    return result;
                }

                // Slices the input by the sequence::parameter::delimeter character
                std::vector<sequence::parameter::numeric> parseParameterSequence(std::string_view input) {
                    std::vector<sequence::parameter::numeric> result;
                    std::vector<size_t> delimeterIndicies;

                    // First fetch all delimeter indicies
                    for (size_t i = 0; i < input.size(); i++) if (input[i] == sequence::parameter::delimeter) delimeterIndicies.push_back(i);

                    // now we can loop through the indicies and pair each [i, i+1], and create parameters
                    for (size_t i = 0; i < delimeterIndicies.size(); i++) {
                        size_t start = delimeterIndicies[i];
                        size_t end = i == delimeterIndicies.size() - 1 ? input.size() : delimeterIndicies[i + 1];

                        size_t tmp = 0;     // Useless in this case, since we already know the sizes.
                        result.push_back({
                            input.substr(start, end), 
                            tmp
                        });
                    }

                    return result;
                }

                std::vector<table::intermediate::identifiers> parseintermediates(std::string_view input) {
                    std::vector<table::intermediate::identifiers> result;

                    // Find all intermediates:
                    for (size_t i = 0; i < input.size() && table::contains<table::intermediate::identifiers>(input[i]); i++) {
                        result.push_back(static_cast<table::intermediate::identifiers>(input[i]));
                    }

                    return result;
                }

                std::pair<prefix*, size_t> parsePostfixForC1(std::string_view input) {
                    table::C1 header = static_cast<table::C1>(input.front());

                    if (header == table::C1::CSI) {
                        // Find the ending function of the current control function introducer
                        size_t finalFunctionAt = 0;
                        size_t intermediateAt = 0;

                        for (size_t i = 1; i < input.size(); i++) {
                            if (table::contains<table::finalWithoutIntermediate>(input[i]) || table::contains<table::finalWithIntermediate>(input[i])) {
                                finalFunctionAt = i;
                                break;
                            } else if (table::contains<table::intermediate::identifiers>(input[i]) && intermediateAt == 0) {    // Only set intermediate once at the first occurrence
                                intermediateAt = i;
                            }
                        }

                        // The final function has to be at a different location than the intermediate
                        bool hasIntermediate = finalFunctionAt > intermediateAt && intermediateAt != 0;
                        std::variant<
                            table::finalWithoutIntermediate,
                            table::finalWithIntermediate
                        > finalFunction;
                        
                        if (hasIntermediate) {
                            finalFunction = static_cast<table::finalWithIntermediate>(input[finalFunctionAt]);
                        } else {
                            finalFunction = static_cast<table::finalWithoutIntermediate>(input[finalFunctionAt]);
                        }

                        // Now we can create a postfix
                        postfix<std::variant<
                            table::finalWithoutIntermediate,
                            table::finalWithIntermediate
                        >> tail(parseintermediates(input.substr(intermediateAt)), finalFunction);

                        // need clarification?
                        size_t earliestNonParametricIndex = std::min(finalFunctionAt, intermediateAt);

                        std::vector<sequence::parameter::numeric> params;

                        // Check if there are any parameters present
                        if (earliestNonParametricIndex != 0) {
                            params = parseParameterSequence(input.substr(earliestNonParametricIndex - 1));
                        }

                        return {
                            new sequence::control<sequence::parameter::numeric>(params, tail),
                            finalFunctionAt + 1
                        };

                    } else if (header == table::C1::APC || header == table::C1::DCS || header == table::C1::OSC || header == table::C1::PM || header == table::C1::SOS) {

                        // TODO: ...
                        return {nullptr, 0};

                    } else {    // TODO: ...
                        return {nullptr, 0};
                    }
                }

                std::pair<prefix*, size_t> parsePostfixForC0(std::string_view input) {
                    table::C0 header = static_cast<table::C0>(input.front());

                    if (header == table::C0::ESC) { // Proceeding bytes cannot be that of CSI, since it would have been captured at the previous parsing stage.

                        size_t startOfIntermediates = 1;

                        // Parse intermediates
                        std::vector<table::intermediate::identifiers> intermediates = parseintermediates(input.substr(startOfIntermediates));

                        sequence::function<table::independentFunctions>* result = new sequence::function<table::independentFunctions>({
                            intermediates,
                            static_cast<table::independentFunctions>(input.at(startOfIntermediates + intermediates.size()))
                        });

                        return {result, startOfIntermediates + intermediates.size()};

                    } else {
                        return {nullptr, 0};
                    }
                }

                // Scans for ecma::sequences::shiftFunctions::* members
                void operateShift(prefix* opcode) {
                    auto layoutType = table::configuration::layout::graphical::type::A;     // TODO: Dynamically adjust this.

                    // First we need to determine the opcode type, via it's header value, C0 or C1 shift function:
                    if (opcode->getType() == sequence::types::SINGLE_BYTE) {
                        // C1{SS2, SS3}, C0{LS0, LS1, SI, SO}, 
                        if (opcode->contains(table::C1::SS2)) {
                            pageState.load(
                                table::configuration::repertoire::G2, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::TEMPORARY
                            );
                        } else if (opcode->contains(table::C1::SS3)) {
                            pageState.load(
                                table::configuration::repertoire::G3, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::TEMPORARY
                            );
                        } else if (opcode->contains(table::C0::LS0)) {
                            pageState.load(
                                table::configuration::repertoire::G0, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        } else if (opcode->contains(table::C0::LS1)) {
                            pageState.load(
                                table::configuration::repertoire::G1, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        }
                    } else if (opcode->getType() == sequence::types::INDEPENDENT_FUNCTION) {
                        // C0{ESC{LS1R, LS2, LS2R, LS3, LS3R}}
                        auto independentFunction = static_cast<sequence::function<table::independentFunctions>*>(opcode);

                        if (independentFunction->getFinalByte() == table::independentFunctions::LS1R) {
                            pageState.load(
                                table::configuration::repertoire::G1, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        } else if (independentFunction->getFinalByte() == table::independentFunctions::LS2) {
                            pageState.load(
                                table::configuration::repertoire::G2, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        } else if (independentFunction->getFinalByte() == table::independentFunctions::LS2R) {
                            pageState.load(
                                table::configuration::repertoire::G2, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        } else if (independentFunction->getFinalByte() == table::independentFunctions::LS3) {
                            pageState.load(
                                table::configuration::repertoire::G3, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        } else if (independentFunction->getFinalByte() == table::independentFunctions::LS3R) {
                            pageState.load(
                                table::configuration::repertoire::G3, 
                                table::configuration::layout::graphical::getRelativeGraphicalPageLayout(layoutType).to8bit(), 
                                table::configuration::lifetime::types::LOCKING
                            );
                        }
                    }
                }

                std::vector<prefix*> parse(std::string_view input) {
                    std::vector<prefix*> result;

                    for (size_t i = 0; i < input.size(); i++) {
                        // First check while temporary loads are active from previous loop
                        auto pageCallReturn = pageState.interpret(input.substr(i));

                        pageState.update();     // refresh temporary pages

                        // Check if the current bytecode triggered some loaded page
                        if (pageCallReturn.first > 0) {
                            result.push_back(pageCallReturn.second);
                            i += pageCallReturn.first;      // TODO: check for maybe adding -1, since the loop increases 'i' either way.
                        } else {    // If not, then check for normal bytecode interpret path:
                            prefix* header = nullptr;

                            // Header prefetch ------------------------------------------------
                            if (table::contains<table::C0>(input[i])) {
                                header = new prefix(static_cast<table::C0>(input[i]));

                                // We can skip ESC and set header to point into C1 if possible
                                if (header->contains(table::C0::ESC)) {     // Now we can check if i+1 contains a C1 bytecode
                                    i++;

                                    // We can promote the ECS + C1 code into a single 8-bit C1 bytecode
                                    if (i < input.size() && table::contains<table::C1>(input[i])) {
                                        header = new prefix(static_cast<table::C1>(input[i]));
                                    }
                                }
                            } else if (table::contains<table::C1>(input[i]))  header = new prefix(static_cast<table::C1>(input[i]));
                            else {  // header == nullptr
                                continue;   // Probably just a graphical character
                            }
                            // ----------------------------------------------------------------


                            // Header Extension -----------------------------------------------
                            std::pair<prefix*, size_t> extension;

                            // Now we can check for extensions of prefix type class:
                            if (std::holds_alternative<table::C1>(header->getFunction())){
                                extension = parsePostfixForC1(input.substr(i));
                            } else {    // table::C0
                                extension = parsePostfixForC0(input.substr(i));
                            }

                            // Even shifts are reported for status checks, put this after shift check to disable shift reporting.
                            if (extension.first != nullptr) {
                                result.push_back(extension.first);
                                i += extension.second;
                            }
                            // ----------------------------------------------------------------


                            // Shift recording ------------------------------------------------
                            operateShift(extension.first);
                            // ----------------------------------------------------------------
                        }
                    }
                    return result;
                }
            }

            std::pair<size_t, sequence::prefix*> table::configuration::manager::interpret(std::string_view input) {
                auto currentRepertoire = map[static_cast<uint8_t>(input.front())];

                // Skip empty jump cells
                if (currentRepertoire == repertoire::NONE) return {0, nullptr};

                // Jump through the page cell
                return pages[static_cast<size_t>(currentRepertoire)].call(input);
            }
        }
    }
}