#include "ecma.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {


            namespace sequence {
                /**
                 * As stated by 5.4.2.b, f, g and h
                 * 
                 * We expect the input to already cut by the 03/11 (';') delimeter by the calling function.
                 * Per char, only be in range of 03/00 - 03/09 or special sub-string delimeter of 03/10 (':')
                 */
                parameter::parameter(std::string_view input, size_t& length) {
                    uint32_t currentNumber = 0;
                    bool has_digit = true;          // Default true, so that ;;;; are possible.

                    for (char i : input) {
                        uint8_t currentChar = static_cast<uint8_t>(i);

                        if (currentChar == sub_delimeter) { // 03/10 ':'
                            if (has_digit) {
                                subNumbers.push_back(currentNumber);
                            } else {
                                subNumbers.push_back(0); // empty sub-string → default / zero
                            }

                            // Reset
                            currentNumber = 0;
                            has_digit = false;
                            length++;
                        } else if (currentChar >= table::toInt(3, 0) && currentChar <= table::toInt(3, 9)) {    // 03/00 - 03/09
                            // Transform the char number into usable form.
                            currentNumber = currentNumber * 10 + (currentChar - table::toInt(3, 0));
                            has_digit = true;
                            length++;
                        } else {
                            break;
                        }
                    }

                    if (has_digit) {
                        subNumbers.push_back(currentNumber);
                        // length++;    <-- no need to increase it since the loop which gathered these numbers already accounts the length.
                        return;
                    } else {    // Trailing 03/10 (':'), stated by section f, needs to have a trailing zero.
                        subNumbers.push_back(0);
                        length++;
                    }
                }

                std::string parameter::toString() {
                    std::string result = "";

                    for (size_t i = 0; i < subNumbers.size(); i++) {
                        uint32_t currentNumber = subNumbers[i]; 
                        
                        if (currentNumber == 0 || i > 0) { 
                            // This means section f was triggered and we need to insert a pre-fix of 03/10
                            // Or this is i+1 so delimeter is required by section b
                            result += static_cast<char>(sub_delimeter); // 03/10 ':'
                            result += std::to_string(currentNumber);
                        } else {
                            result += std::to_string(currentNumber);
                        }
                    }

                    return result;
                }
            
                // First declare some tools for us
                auto C0Present =                    [](char value){ return table::contains<table::C0>(value); };
                auto C1_8bitPresent =               [](char value){ return table::checkBit(value, 7) && table::contains<table::C1>(table::shiftColumns(value, table::columns::FOUR, true)); };
                auto CSIPresent =                   [](char value){ return table::is(value, table::C1::CSI) || table::is(value, table::to8bit(table::C1::CSI)); };
                auto ESCPresent =                   [](char value){ return table::is(value, table::C0::ESC); };
                auto independentFunctionPresent =   [](char value){ return table::contains<table::independentFunctions>(value); };
                auto intermediateSpacePresent =     [](char value){ return static_cast<uint8_t>(value) == table::toInt(2, 3); };
                auto controlStringPresent =         [](char value){ return (
                    table::is(value, table::C1::OSC) ||
                    table::is(value, table::C1::DCS) ||
                    table::is(value, table::C1::APC) ||
                    table::is(value, table::C1::PM) ||
                    table::is(value, table::C1::SOS));
                };
                auto finalControlSequenceBytePresent = [](char value) { return table::contains<table::finalWithIntermediate>(value) || table::contains<table::finalWithoutIntermediate>(value); };
                auto findIndexOffFinalByteForControlSequence = [](std::string_view input) {
                    for (size_t i = 0; i < input.size(); i++) {
                        if (finalControlSequenceBytePresent(input[i])) {
                            return i;
                        }
                    }

                    return std::string_view::npos;
                };

                void basic::parse(std::string_view input, size_t& length, std::vector<base*>& output) {
                    if (input.empty()) return;

                    size_t functionPosition = 0;

                    if (C1_8bitPresent(input[functionPosition])) {
                        output.push_back(new basic(
                            static_cast<table::C1>(table::shiftColumns(static_cast<uint8_t>(input[functionPosition]), table::columns::FOUR, true))
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
                                static_cast<table::C1>(input[functionPosition])
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

                void controlSequence::parse(std::string_view input, size_t& length, std::vector<base*>& output) {
                    // Check for 7-bit variant
                    size_t headerPosition = 0;
                    bool _7bitHeaders = ESCPresent(input[headerPosition]) && input.size() > 1 && CSIPresent(input[headerPosition + 1]) && !C1_8bitPresent(input[headerPosition + 1]);
                    bool _8bitHeaders = C1_8bitPresent(input[headerPosition]) && CSIPresent(input[headerPosition]);
                    
                    if (!_7bitHeaders && !_8bitHeaders) return;

                    // This is where the parameters will start if any
                    size_t contentOffset = _7bitHeaders ? 2 : 1;
                    
                    // Now we need to find for the end of this control sequence, either by a an intermediated final byte or a non-intermediated final byte.
                    size_t finalByteOffset = findIndexOffFinalByteForControlSequence(input.substr(contentOffset));

                    if (finalByteOffset == std::string_view::npos) return; // unbound sequence.

                    // Since we know the end if this control sequence we can already calculate the final length here:
                    length += contentOffset + finalByteOffset + 1; // ESC(+/)CSI ... Fs

                    // finalByteOffset is found, it is thus within bounds.
                    uint8_t finalByte = static_cast<uint8_t>(input[contentOffset + finalByteOffset]);

                    /** Now we need to figure out how many parameters if any are present, these sequence variants can be as follows:
                    * - (finalByteOffset - contentOffset) == 0:
                    *   - TRUE: No parameters or intermediates, return.
                    *   - FALSE: input.substr(contentOffset).split(';') == 0
                    *       - TRUE: we check for: hasIntermediateBytesPresent(input.substr(contentOffset)) == true
                    *           - TRUE: no parameters present, only intermediate bytes
                    *           - FALSE: only one parameter present and no intermediates
                    *       - FALSE: 
                    * -         - more than one parameter present, split them and feed the splitted string to the parameter class-
                    *             the last parameters length is then used to determine if there is intermediate bytes at the end or not.
                    */

                    if (finalByteOffset == 0) {
                        // No parameters or intermediates
                        output.push_back(
                            new controlSequence(
                                finalByteOffset,
                                static_cast<table::finalWithoutIntermediate>(finalByte)
                            )
                        );
                        return;
                    } else {
                        std::string_view content = input.substr(contentOffset, finalByteOffset);
                        std::vector<parameter> parameters;
                        std::vector<uint8_t> intermediates;

                        size_t parsedLength = 0;

                        // Split by ';' (03/11)
                        while (parsedLength < content.size()) {
                            size_t nextDelimeterIndex = content.find_first_of(parameter::delimeter, parsedLength);
                            
                            if (nextDelimeterIndex != std::string_view::npos) {
                                // Found delimiter, parse parameter up to delimiter
                                size_t tmp = 0;
                                parameters.push_back(parameter(content.substr(parsedLength, nextDelimeterIndex - parsedLength), tmp));
                                parsedLength = nextDelimeterIndex + 1; // Skip past the delimiter
                            } else {
                                // No more delimeter, parse remaining content
                                parameter param(content.substr(parsedLength), parsedLength);
                                parameters.push_back(param);
                                break;
                            }
                        }

                        if (parsedLength < content.size()) {
                            // There are intermediate bytes
                            intermediates = std::vector<uint8_t>(content.substr(parsedLength).begin(), content.substr(parsedLength).end());

                            output.push_back(
                                new controlSequence(
                                    parameters,
                                    intermediates,
                                    static_cast<table::finalWithIntermediate>(finalByte)
                                )
                            );
                        } else {
                            // No intermediate bytes
                            output.push_back(
                                new controlSequence(
                                    parameters,
                                    static_cast<table::finalWithoutIntermediate>(finalByte)
                                )
                            );
                        }
                    }
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
                        controlSequence::parse(input.substr(i), skipFor, result);
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