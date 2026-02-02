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
                parameter::parameter(std::string_view input) {
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
                        } else {
                            // Transform the char number into usable form.
                            currentNumber = currentNumber * 10 + (currentChar - table::toInt(3, 0));
                            has_digit = true;
                        }
                    }

                    if (has_digit) {
                        if (currentNumber > 0) {
                            subNumbers.push_back(currentNumber);
                        }

                        return;
                    } else {    // Trailing 03/10 (':'), stated by section f, needs to have a trailing zero.
                        subNumbers.push_back(0);
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
            }
        }
    }
}