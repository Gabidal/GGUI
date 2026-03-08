#include "ecma.h"

namespace GGUI {
    namespace terminal {
        namespace ecma {
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

                std::vector<prefix*> parse(std::string_view input) {
                    std::vector<prefix*> result;

                    for (size_t i = 0; i < input.size();) {
                        size_t skipFor = 0;

                        

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