#include "ecma.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {


            namespace sequence {
                /**
                 * As stated by 5.4.2.b, f, g and h
                 * 
                 * We expect the input to already cut by the 03/11 (';') delimiters by the calling function.
                 * Per char, only be in range of 03/00 - 03/09 or special sub-string delimeter of 03/10 (':')
                 */
                parameter::parameter(std::string_view input, int& length) {
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
                        if (currentNumber > 0) {
                            subNumbers.push_back(currentNumber);
                            length++;
                        }

                        return;
                    } else {    // Trailing 03/10 (':'), stated by section f, needs to have a trailing zero.
                        subNumbers.push_back(0);
                        length++;
                    }
                }

                std::string parameter::toString() {
                    std::string result = "";

                    for (int i = 0; i < subNumbers.size(); i++) {
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
                }
            
                std::vector<base*> parse(std::string_view input) {
                    std::vector<base*> result;

                    for (int i = 0; i < input.size(); i++) {
                        /** Parsing rules:
                        *  - Single-byte functions are either C0 or 8-bit C1 table functions.
                        *  - C1 functions "contain" ESC prefix in them.
                        *  - Only ESC from C0 enables use of C1 functions.
                        *  - Only CSI from C1 (7 or 8-bit) enables control sequences.
                        */ 

                        // First declare some tools for us
                        auto C0Present =                    [&input](int index){ return index < input.size() && table::contains<table::C0>(input[index]); };
                        auto C1_8bitPresent =               [&input](int index){ return index < input.size() && table::checkBit(input[index], 7); };
                        auto CSIPresent =                   [&input](int index){ return index < input.size() && table::is(input[index], table::C1::CSI) || table::is(input[index], table::to8bit(table::C1::CSI)); };
                        auto ESCPresent =                   [&input](int index){ return index < input.size() && table::is(input[index], table::C0::ESC); };
                        auto independentFunctionPresent =   [&input](int index){ return index < input.size() && table::contains<table::independentFunctions>(input[index]); };
                        auto controlStringPresent =         [&input](int index){ return index < input.size() && (
                            table::is(input[index], table::C1::OSC) ||
                            table::is(input[index], table::C1::DCS) ||
                            table::is(input[index], table::C1::APC) ||
                            table::is(input[index], table::C1::PM) ||
                            table::is(input[index], table::C1::SOS));
                        };

                        int skipFor = 0;

                        if ((C0Present(i) || C1_8bitPresent(i)) && !CSIPresent(i) && !ESCPresent(i)) {  // Basic functions C0/C1

                            result.push_back(new basic(input.substr(i), skipFor));

                        } else if (ESCPresent(i) && independentFunctionPresent(i+1)) {    // Independent functions

                            result.push_back(new independent(input.substr(i), skipFor));

                        } else if ((ESCPresent(i) && CSIPresent(i+1)) && !C1_8bitPresent(i+1) || (C1_8bitPresent(i) && CSIPresent(i))) {    // Control sequences

                            result.push_back(new controlSequence(input.substr(i), skipFor));

                        } else if ((ESCPresent(i) && !C1_8bitPresent(i+1) && controlStringPresent(i+1)) || (C1_8bitPresent(i) && controlStringPresent(i))) {    // Control strings

                            result.push_back(new controlString(input.substr(i), skipFor));

                        } else {
                            // Unknown
                        }

                        i += skipFor;
                    }

                    return result;
                }
            }
        }
    }
}