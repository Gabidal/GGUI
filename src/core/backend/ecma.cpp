#include "ecma.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {
            namespace table {
                namespace mode {
                    flags base::operator|(base other) const {
                        return flags(*this) | flags(other);
                    }
                }
            }

            namespace sequence {
                using namespace sequence::INTERNAL;

                void basic::parse(std::string_view input, size_t& length, std::vector<base*>& output) {
                    if (input.empty()) return;

                    size_t functionPosition = 0;

                    if (C1_8bitPresent(input[functionPosition])) {
                        output.push_back(new basic(
                            static_cast<table::C1>(table::shiftColumns(static_cast<uint8_t>(input[functionPosition]), table::columns::FOUR, true)),
                            bitType::_8bit
                        ));
                        length++;
                    } else if (ESCPresent(input[functionPosition])) {
                        if (input.size() < 2) return;

                        functionPosition++;

                        if (CSIPresent(input[functionPosition]) || independentFunctionPresent(input[functionPosition]) || intermediateSpacePresent(input[functionPosition])) {
                            // This is a control sequence, or independent function
                            return;
                        }

                        if (table::contains<table::C1>(input[functionPosition])) {
                            output.push_back(new basic(
                                static_cast<table::C1>(input[functionPosition]),
                                bitType::_7bit
                            ));

                            length += 1 + functionPosition;
                        }
                    } else if (table::contains<table::C0>(input[functionPosition])) {
                        // ESC is not present, otherwise the previous condition would have been triggered
                        output.push_back(new basic(
                            static_cast<table::C0>(input[functionPosition])
                        ));
                    }
                }

                /**
                 * Converts the basic function to its string representation.
                 * 
                 * For C0 functions, the byte is directly output.
                 * For C1 functions, based on escapeType:
                 *   - 7-bit: ESC (01/11) followed by the C1 function byte from table 2a (columns 4-5)
                 *   - 8-bit: The C1 function byte shifted to table 2b (columns 8-9)
                 */
                std::string basic::toString() {
                    std::string result = "";

                    if (std::holds_alternative<table::C0>(function)) {
                        // C0 functions are single-byte, directly output
                        result += static_cast<char>(std::get<table::C0>(function));
                    } else {
                        // C1 functions require either ESC prefix (7-bit) or column-shifted byte (8-bit)
                        table::C1 functionValue = std::get<table::C1>(function);

                        if (escapeType == bitType::_7bit) {
                            // 7-bit representation: ESC + C1 byte from table 2a
                            result += static_cast<char>(table::C0::ESC);
                            result += static_cast<char>(functionValue);
                        } else {
                            // 8-bit representation: C1 byte shifted from columns 4-5 to columns 8-9
                            result += static_cast<char>(table::to8bit(functionValue));
                        }
                    }

                    return result;
                }

                void independent::parse(std::string_view input, size_t& length, std::vector<base*>& output) {
                    if (!ESCPresent(input[0])) return;

                    if (input.size() < 2) return;

                    size_t functionPosition = 1;
                    size_t intermediateOffset = intermediateSpacePresent(input[functionPosition]) ? 1 : 0;

                    if (input.size() < 1 + intermediateOffset + functionPosition) return;

                    if (!independentFunctionPresent(input[intermediateOffset + functionPosition])) return;

                    output.push_back(new independent(
                        static_cast<table::independentFunctions>(input[intermediateOffset + functionPosition]),
                        intermediateOffset == 1
                    ));

                    length += 1 + intermediateOffset + functionPosition;
                }

                /**
                 * Converts the independent function to its string representation.
                 * 
                 * Independent functions are always 7-bit and follow the format:
                 *   ESC (01/11) + optional space (02/03) + function byte from table 5 (columns 6-7)
                 */
                std::string independent::toString() {
                    std::string result = "";

                    // Independent functions always start with ESC
                    result += static_cast<char>(table::C0::ESC);

                    // Optional intermediate space byte (02/03)
                    if (hasSpace) {
                        result += static_cast<char>(table::toInt(2, 3));
                    }

                    // The function byte from the independentFunctions table
                    result += static_cast<char>(function);

                    return result;
                }

                void controlString::parse(std::string_view input, size_t& length, std::vector<base*>& output) {
                    if (input.size() < 2) return;

                    size_t headerPosition = 0;
                    bool _8bitC1ControlStringPreset = C1_8bitPresent(input[headerPosition]) && controlStringPresent(input[headerPosition]);
                    bool _7bitC0AndControlStringPreset = ESCPresent(input[headerPosition]) && !C1_8bitPresent(input[headerPosition + 1]) && controlStringPresent(input[headerPosition + 1]);

                    if (!_8bitC1ControlStringPreset && !_7bitC0AndControlStringPreset) return;

                    size_t contentOffset = _7bitC0AndControlStringPreset ? 2 : 1;

                    size_t stringTerminatorOffset = std::string_view::npos;

                    // We can safely assume that there must be the ST character at the end of this sequence
                    if (_7bitC0AndControlStringPreset) stringTerminatorOffset = input.find_first_of(static_cast<char>(table::C1::ST), contentOffset);
                    else stringTerminatorOffset = input.find_first_of(static_cast<char>(table::to8bit(table::C1::ST)), contentOffset);

                    if (stringTerminatorOffset == std::string_view::npos) return; // unbound sequence

                    output.push_back(new controlString(
                        static_cast<table::C1>(input[contentOffset-1]), // -1 to go one backwards which should always be the opening delimeter.
                        std::vector<uint8_t>(
                            input.substr(contentOffset, stringTerminatorOffset - contentOffset).begin(),
                            input.substr(contentOffset, stringTerminatorOffset - contentOffset).end()
                        )
                    ));

                    length += stringTerminatorOffset + 1; // +1 for the ST character
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
                std::string controlString::toString() {
                    std::string result = "";

                    // Output the opening delimiter based on escape type
                    if (escapeType == bitType::_7bit) {
                        // 7-bit representation: ESC + opening delimiter from table 2a
                        result += static_cast<char>(table::C0::ESC);
                        result += static_cast<char>(openingDelimiter);
                    } else {
                        // 8-bit representation: opening delimiter shifted to columns 8-9
                        result += static_cast<char>(table::to8bit(openingDelimiter));
                    }

                    // Output all character bytes
                    for (uint8_t characterByte : characters) {
                        result += static_cast<char>(characterByte);
                    }

                    // Output the String Terminator (ST) based on escape type
                    if (escapeType == bitType::_7bit) {
                        // 7-bit representation: ESC + ST from table 2a
                        result += static_cast<char>(table::C0::ESC);
                        result += static_cast<char>(table::C1::ST);
                    } else {
                        // 8-bit representation: ST shifted to columns 8-9
                        result += static_cast<char>(table::to8bit(table::C1::ST));
                    }

                    return result;
                }

                std::vector<base*> parse(std::string_view input) {
                    std::vector<base*> result;

                    for (size_t i = 0; i < input.size();) {
                        /** Parsing rules:
                        *  - Single-byte functions are either C0 or 8-bit C1 table functions.
                        *  - C1 functions "contain" ESC prefix in them.
                        *  - Only ESC from C0 enables use of C1 functions.
                        *  - Only CSI from C1 (7 or 8-bit) enables control sequences.
                        */ 

                        size_t skipFor = 0;

                        basic::parse(input.substr(i), skipFor, result);
                        independent::parse(input.substr(i), skipFor, result);
                        controlSequence<sequence::parameter::numeric>::parse(input.substr(i), skipFor, result);
                        controlString::parse(input.substr(i), skipFor, result);

                        // Check if current character is non-parsable
                        if (skipFor == 0) {
                            i++;    // go next
                        }
                        else {
                            i += skipFor;
                        }
                    }
                    return result;
                }
            }
        }
    }
}