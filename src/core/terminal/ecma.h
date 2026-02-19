#ifndef _ECMA_H_
#define _ECMA_H_

#include "../utils/types.h"
#include "terminal.h"

/**
 * Source: https://ecma-international.org/wp-content/uploads/ECMA-48_5th_edition_june_1991.pdf
*/

namespace GGUI {
    namespace terminal {
        namespace ecma {

            namespace table {
                constexpr uint8_t tableRows = 16;

                constexpr uint8_t toInt(uint8_t column, uint8_t row) {
                    return (column * tableRows) + row;
                }

                enum class columns : int8_t {
                    ZERO        = 0,
                    ONE         = 1,

                    FOUR        = 4,
                    FIVE        = 5,

                    EIGHT       = 8,
                    NINE        = 9,
                };
    
                // This is used to convert between C1 7bit and 8bit variants.
                constexpr uint8_t shiftColumns(uint8_t value, columns by, bool down = false) {
                    const int16_t sign = down ? -1 : 1;

                    return static_cast<uint8_t>(
                        static_cast<int16_t>(value) + (sign * static_cast<int16_t>(by) * static_cast<int16_t>(tableRows))
                    );
                }

                /**
                 * @brief Checks if a value falls within the range defined by an enum's __min and __max members.
                 * 
                 * @tparam E The enum type that defines __min and __max bounds
                 * @param val The value to check against the enum's range
                 * @return true if val is within the inclusive range [E::__min, E::__max]
                 * @return false otherwise
                 */
                template<typename E, typename V>
                bool contains(V val) {
                    return val >= static_cast<uint8_t>(E::__min) && val <= static_cast<uint8_t>(E::__max);
                }

                template<typename V, typename E>
                bool is(V val, E selected) {
                    return static_cast<uint8_t>(val) == static_cast<uint8_t>(selected);
                }

                constexpr bool checkBit(uint8_t val, int i) {
                    return (val & (1 << i)) != 0;
                }

                enum class C0 : uint8_t {   // Table 1
                    __min = toInt(0, 0),  // For internal automation
                    // Column 0             // Column 1
                    NUL = toInt(0, 0),      DLE = toInt(1, 0),
                    SOH = toInt(0, 1),      DC1 = toInt(1, 1),
                    STX = toInt(0, 2),      DC2 = toInt(1, 2),
                    ETX = toInt(0, 3),      DC3 = toInt(1, 3),
                    EOT = toInt(0, 4),      DC4 = toInt(1, 4),
                    ENQ = toInt(0, 5),      NAK = toInt(1, 5),
                    ACK = toInt(0, 6),      SYN = toInt(1, 6),
                    BEL = toInt(0, 7),      ETB = toInt(1, 7),
                    BS  = toInt(0, 8),      CAN = toInt(1, 8),
                    HT  = toInt(0, 9),      EM  = toInt(1, 9),
                    LF  = toInt(0, 10),     SUB = toInt(1, 10),
                    VT  = toInt(0, 11),     ESC = toInt(1, 11),
                    FF  = toInt(0, 12),     IS4 = toInt(1, 12),
                    CR  = toInt(0, 13),     IS3 = toInt(1, 13),
                    LS1 = toInt(0, 14),     IS2 = toInt(1, 14),     SO = toInt(0, 14),  // <-- legacy
                    LS0 = toInt(0, 15),     IS1 = toInt(1, 15),     SI = toInt(0, 15),  // <-- legacy

                    __max = toInt(1, 15)   // For internal automation
                };

                // NOTE: This is the 7bit table from table 2a. Use the column shifter to switch between the 2a and 2b tables in C1.
                enum class C1 : uint8_t {   // Table 2
                    __min = toInt(4, 0),  // For internal automation
                    // Column 4             // Column 5
                    /*   --   */            DCS = toInt(5, 0),
                    /*   --   */            PU1 = toInt(5, 1),
                    BPH = toInt(4, 2),      PU2 = toInt(5, 2),
                    NBH = toInt(4, 3),      STS = toInt(5, 3),
                    /*   --   */            CCH = toInt(5, 4),
                    NEL = toInt(4, 5),      MW  = toInt(5, 5),
                    SSA = toInt(4, 6),      SPA = toInt(5, 6),
                    ESA = toInt(4, 7),      EPA = toInt(5, 7),
                    HTS = toInt(4, 8),      SOS = toInt(5, 8),
                    HTJ = toInt(4, 9),      /*   --   */
                    VTS = toInt(4, 10),     SCI = toInt(5, 10),
                    PLD = toInt(4, 11),     CSI = toInt(5, 11),
                    PLU = toInt(4, 12),     ST  = toInt(5, 12),
                    RI  = toInt(4, 13),     OSC = toInt(5, 13),
                    SS2 = toInt(4, 14),     PM  = toInt(5, 14),
                    SS3 = toInt(4, 15),     APC = toInt(5, 15),

                    __max = toInt(5, 15)   // For internal automation
                };

                // Small helper function for conversion between C1 7-bit and 8-bit variants.
                constexpr uint8_t to8bit(C1 value) {
                    return shiftColumns(static_cast<uint8_t>(value), columns::FOUR, false);
                }

                enum class finalWithoutIntermediate : uint8_t {   // Table 3 Final Bytes of control sequences without Intermediate Bytes 
                    __min = toInt(4, 0),  // For internal automation

                    // Column 4             // Column 5             // Column 6             // Column 7
                    ICH = toInt(4, 0),      DCH = toInt(5, 0),      HPA = toInt(6, 0),      /*   --   */
                    CUU = toInt(4, 1),      SSE = toInt(5, 1),      HPR = toInt(6, 1),      /*   --   */
                    CUD = toInt(4, 2),      CPR = toInt(5, 2),      REP = toInt(6, 2),      /*   --   */
                    CUF = toInt(4, 3),      SU  = toInt(5, 3),      DA  = toInt(6, 3),      /*   --   */
                    CUB = toInt(4, 4),      SD  = toInt(5, 4),      VPA = toInt(6, 4),      /*   --   */
                    CNL = toInt(4, 5),      NP  = toInt(5, 5),      VPR = toInt(6, 5),      /*   --   */
                    CPL = toInt(4, 6),      PP  = toInt(5, 6),      HVP = toInt(6, 6),      /*   --   */
                    CHA = toInt(4, 7),      CTC = toInt(5, 7),      TBC = toInt(6, 7),      /*   --   */
                    CUP = toInt(4, 8),      ECH = toInt(5, 8),      SM  = toInt(6, 8),      /*   --   */
                    CHT = toInt(4, 9),      CVT = toInt(5, 9),      MC  = toInt(6, 9),      /*   --   */
                    ED  = toInt(4, 10),     CBT = toInt(5, 10),     HPB = toInt(6, 10),     /*   --   */
                    EL  = toInt(4, 11),     SRS = toInt(5, 11),     VPB = toInt(6, 11),     /*   --   */
                    IL  = toInt(4, 12),     PTX = toInt(5, 12),     RM  = toInt(6, 12),     /*   --   */
                    DL  = toInt(4, 13),     SDS = toInt(5, 13),     SGR = toInt(6, 13),     /*   --   */
                    EF  = toInt(4, 14),     SIMD = toInt(5, 14),    DSR = toInt(6, 14),     /*   --   */
                    EA  = toInt(4, 15),     /*   --   */            DAQ = toInt(6, 15),     /*   --   */

                    __max = toInt(7, 15)   // For internal automation
                };

                enum class finalWithIntermediate : uint8_t {   // Table 4 - Final Bytes of control sequences with a single Intermediate index: (2, 0)
                    __min = toInt(4, 0),  // For internal automation

                    // Column 4             // Column 5             // Column 6             // Column 7
                    SL  = toInt(4, 0),      PPA = toInt(5, 0),      TATE = toInt(6, 0),     /*   --   */
                    SR  = toInt(4, 1),      PPR = toInt(5, 1),      TALE = toInt(6, 1),     /*   --   */
                    GSM = toInt(4, 2),      PPB = toInt(5, 2),      TAC  = toInt(6, 2),     /*   --   */
                    GSS = toInt(4, 3),      SPD = toInt(5, 3),      TCC  = toInt(6, 3),     /*   --   */
                    FNT = toInt(4, 4),      DTA = toInt(5, 4),      TSR  = toInt(6, 4),     /*   --   */
                    TSS = toInt(4, 5),      SHL = toInt(5, 5),      SCO  = toInt(6, 5),     /*   --   */
                    JFY = toInt(4, 6),      SLL = toInt(5, 6),      SRCS = toInt(6, 6),     /*   --   */
                    SPI = toInt(4, 7),      FNK = toInt(5, 7),      SCS  = toInt(6, 7),     /*   --   */
                    QUAD = toInt(4, 8),     SPQR = toInt(5, 8),     SLS  = toInt(6, 8),     /*   --   */
                    SSU = toInt(4, 9),      SEF = toInt(5, 9),      /*   --   */            /*   --   */
                    PFS = toInt(4, 10),     PEC = toInt(5, 10),     /*   --   */            /*   --   */
                    SHS = toInt(4, 11),     SSW = toInt(5, 11),     SCP  = toInt(6, 11),    /*   --   */
                    SVS = toInt(4, 12),     SACS = toInt(5, 12),    /*   --   */            /*   --   */
                    IGS = toInt(4, 13),     SAPV = toInt(5, 13),    /*   --   */            /*   --   */
                    /*   --   */            STAB = toInt(5, 14),    /*   --   */            /*   --   */
                    IDCS = toInt(4, 15),    GCC  = toInt(5, 15),    /*   --   */            /*   --   */

                    __max = toInt(7, 15)   // For internal automation
                };

                enum class independentFunctions : uint8_t { // Table 5 - Independent control functions 
                    __min = toInt(6, 0),  // For internal automation

                    // Column 6             // Column 7
                    DMI = toInt(6, 0),      /*   --   */
                    INT = toInt(6, 1),      /*   --   */
                    EMI = toInt(6, 2),      /*   --   */
                    RIS = toInt(6, 3),      /*   --   */
                    CMD = toInt(6, 4),      /*   --   */
                    // ...
                    /*   --   */            LS3R = toInt(7, 12),
                    /*   --   */            LS2R = toInt(7, 13),
                    LS2 = toInt(6, 14),     LS1R = toInt(7, 14),
                    LS3 = toInt(6, 15),     /*   --   */

                    __max = toInt(7, 15)   // For internal automation
                };
            }

            namespace sequence {
                enum class bitType {
                    _7bit,          // For the 1/11 introducers
                    _8bit           // For the 09/11 introducers
                };

                enum class specialType {
                    BASIC,                          // Contains the single-byte C0 and C1 functions
                    CONTROL_SEQUENCE,               // CSI based functions
                    INDEPENDENT,                    // Contains ESC Fs or ESC 02/03 F functions
                    CONTROL_STRING,                 // Contains the control string functions: OSC, DCS, APC, PM
                };

                // TODO: make this a inheritable simpler base type which numeric and selective classes inherit
                namespace parameter {
                    constexpr static uint8_t sub_delimeter  = table::toInt(3, 10); // Translates into ':'
                    constexpr static uint8_t delimeter      = table::toInt(3, 11); // Translates into ';'

                    template<typename containerType>
                    class base {
                    protected:
                        std::vector<containerType> subNumbers;       // For instances where 1:2, these can be used as decimals.
                    
                    public:
                        base() = default;
                        base(std::vector<containerType> values) : subNumbers(values) {}
                        base(containerType values) : subNumbers({values}) {}

                        /**
                        * As stated by 5.4.2.b, f, g and h
                        * 
                        * We expect the input to already cut by the 03/11 (';') delimeter by the calling function.
                        * Per char, only be in range of 03/00 - 03/09 or special sub-string delimeter of 03/10 (':')
                        */
                        base(std::string_view input, size_t& length) {
                            uint32_t currentNumber = 0;
                            bool has_digit = true;          // Default true, so that ;;;; are possible.

                            for (char i : input) {
                                uint8_t currentChar = static_cast<uint8_t>(i);

                                if (currentChar == sub_delimeter) { // 03/10 ':'
                                    if (has_digit) {
                                        subNumbers.push_back(static_cast<containerType>(currentNumber));
                                    } else {
                                        subNumbers.push_back(static_cast<containerType>(0)); // empty sub-string → default / zero
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
                                subNumbers.push_back(static_cast<containerType>(currentNumber));
                                // length++;    <-- no need to increase it since the loop which gathered these numbers already accounts the length.
                                return;
                            } else {    // Trailing 03/10 (':'), stated by section f, needs to have a trailing zero.
                                subNumbers.push_back(static_cast<containerType>(0));
                                length++;
                            }
                        }

                        std::string toString() const {
                            std::string result = "";

                            for (size_t i = 0; i < subNumbers.size(); i++) {
                                uint32_t currentNumber = static_cast<uint32_t>(subNumbers[i]); 
                                
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
                    };

                    using numeric = base<uint32_t>;

                    template<typename enumType>
                    class selectable : public base<enumType> {
                        // Check that selectable instances are only used with enums
                        static_assert(std::is_enum_v<enumType> == true, "Selectable parameters must be instantiated with an enum type.");
                    };

                }

                class base {
                public:
                    specialType type;                       // Tells what kind of function this is
                    bitType escapeType = bitType::_7bit;    // Since most terminal emulators use the 7bit introducer.

                    virtual std::string toString() { return ""; }

                    base() {}             // Default constructor
                    virtual ~base() {}    // 

                    base(specialType t, bitType b) : type(t), escapeType(b) {}
                };

                class basic : public base {
                protected:
                    std::variant<table::C0, table::C1> function;
                public:
                    static void parse(std::string_view input, size_t& length, std::vector<base*>& output);

                    basic(table::C0 func) : base(specialType::BASIC, bitType::_7bit), function(func) {}
                    basic(table::C1 func, bitType variant = bitType::_7bit) : base(specialType::BASIC, variant), function(func) {}

                    std::string toString() override;
                };

                // Helpers
                inline const auto C0Present =                    [](char value){ return table::contains<table::C0>(value); };
                inline const auto C1_8bitPresent =               [](char value){ return table::checkBit(value, 7) && table::contains<table::C1>(table::shiftColumns(value, table::columns::FOUR, true)); };
                inline const auto CSIPresent =                   [](char value){ return table::is(value, table::C1::CSI) || table::is(value, table::to8bit(table::C1::CSI)); };
                inline const auto ESCPresent =                   [](char value){ return table::is(value, table::C0::ESC); };
                inline const auto independentFunctionPresent =   [](char value){ return table::contains<table::independentFunctions>(value); };
                inline const auto intermediateSpacePresent =     [](char value){ return static_cast<uint8_t>(value) == table::toInt(2, 3); };
                inline const auto controlStringPresent =         [](char value){ return (
                    table::is(value, table::C1::OSC) ||
                    table::is(value, table::C1::DCS) ||
                    table::is(value, table::C1::APC) ||
                    table::is(value, table::C1::PM) ||
                    table::is(value, table::C1::SOS));
                };
                inline const auto finalControlSequenceBytePresent = [](char value) { return table::contains<table::finalWithIntermediate>(value) || table::contains<table::finalWithoutIntermediate>(value); };
                inline const auto findIndexOffFinalByteForControlSequence = [](std::string_view input) {
                    for (size_t i = 0; i < input.size(); i++) {
                        if (finalControlSequenceBytePresent(input[i])) {
                            return i;
                        }
                    }

                    return std::string_view::npos;
                };

                template<typename containerType>
                class controlSequence : public base {
                protected:
                    std::vector<containerType> parameters;         // Each range between: 03/00 - 03/15, delimeetered by 03/11 (';')
                    std::vector<uint8_t> intermediates;                             // Each range between: 02/00 - 02/15
                    std::variant<
                        table::finalWithoutIntermediate,                            // Each range between: 04/00 - 07/14
                        table::finalWithIntermediate                                // Each range between: 04/00 - 07/14
                    > finalByte;
                public:
                    static void parse(std::string_view input, size_t& length, std::vector<base*>& output) {
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
                                    static_cast<table::finalWithoutIntermediate>(finalByte)
                                )
                            );
                            return;
                        } else {
                            std::string_view content = input.substr(contentOffset, finalByteOffset);
                            std::vector<containerType> parameters;
                            std::vector<uint8_t> intermediates;

                            size_t parsedLength = 0;

                            // Split by ';' (03/11)
                            while (parsedLength < content.size()) {
                                size_t nextDelimeterIndex = content.find_first_of(parameter::delimeter, parsedLength);
                                
                                if (nextDelimeterIndex != std::string_view::npos) {
                                    // Found delimiter, parse parameter up to delimiter
                                    size_t tmp = 0;
                                    parameters.push_back(containerType(content.substr(parsedLength, nextDelimeterIndex - parsedLength), tmp));
                                    parsedLength = nextDelimeterIndex + 1; // Skip past the delimiter
                                } else {
                                    // No more delimeter, parse remaining content
                                    containerType param(content.substr(parsedLength), parsedLength);
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

                    controlSequence(
                        std::vector<parameter::base<containerType>> params,
                        table::finalWithoutIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameters(params), finalByte(finalByte) {}

                    controlSequence(
                        std::vector<parameter::base<containerType>> params,
                        std::vector<uint8_t> inters,
                        table::finalWithIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameters(params), intermediates(inters), finalByte(finalByte) {}

                    controlSequence(
                        table::finalWithoutIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), finalByte(finalByte) {}

                    controlSequence(
                        table::finalWithIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), intermediates({table::toInt(2, 0)}), finalByte(finalByte) {}

                    /**
                    * Converts the control sequence to its string representation.
                    * 
                    * Control sequences follow the format:
                    *   - 7-bit: ESC (01/11) + CSI byte (05/11) + parameters + intermediates + final byte
                    *   - 8-bit: 8-bit CSI (09/11) + parameters + intermediates + final byte
                    * 
                    * Parameters are separated by ';' (03/11) delimiter.
                    * The final byte determines if intermediates are required based on the variant held.
                    */
                    std::string toString() override {
                        std::string result = "";

                        // Output the Control Sequence Introducer based on escape type
                        if (escapeType == bitType::_7bit) {
                            // 7-bit representation: ESC + CSI from table 2a
                            result += static_cast<char>(table::C0::ESC);
                            result += static_cast<char>(table::C1::CSI);
                        } else {
                            // 8-bit representation: CSI shifted to column 8-9
                            result += static_cast<char>(table::to8bit(table::C1::CSI));
                        }

                        // Output all parameters, separated by the parameter delimiter (03/11 ';')
                        for (size_t parameterIndex = 0; parameterIndex < parameters.size(); parameterIndex++) {
                            if (parameterIndex > 0) {
                                result += static_cast<char>(parameter::delimeter);
                            }
                            result += parameters[parameterIndex].toString();
                        }

                        // Output all intermediate bytes (range 02/00 - 02/15)
                        for (uint8_t intermediateByte : intermediates) {
                            result += static_cast<char>(intermediateByte);
                        }

                        // Output the final byte based on which variant is held
                        if (std::holds_alternative<table::finalWithoutIntermediate>(finalByte)) {
                            result += static_cast<char>(std::get<table::finalWithoutIntermediate>(finalByte));
                        } else {
                            result += static_cast<char>(std::get<table::finalWithIntermediate>(finalByte));
                        }

                        return result;
                    }
                };

                class independent : public base {
                protected:
                    bool hasSpace = false;                      // Can be: 02/03 (' ')
                    table::independentFunctions function;
                public:
                    static void parse(std::string_view input, size_t& length, std::vector<base*>& output);

                    independent(
                        table::independentFunctions func,
                        bool space = false
                    ) : base(specialType::INDEPENDENT, bitType::_7bit), hasSpace(space), function(func) {}

                    std::string toString() override;
                };

                class controlString : public base {
                protected:
                    table::C1 openingDelimiter;                 // APC, DCS, OSC, PM or SOS
                    std::vector<uint8_t> characters;            // For when SOS is used in the opening delimeter, can contain in range of 00/00 to 07/15 or the command strings In the range 00/08 to 00/13 and 02/00 to 07/14
                public:
                    static void parse(std::string_view input, size_t& length, std::vector<base*>& output);
                    
                    controlString(
                        table::C1 delimeter,
                        std::vector<uint8_t> chars
                    ) : base(specialType::CONTROL_STRING, bitType::_7bit), openingDelimiter(delimeter), characters(chars) {}

                    controlString(
                        table::C1 delimeter
                    ) : base(specialType::CONTROL_STRING, bitType::_7bit), openingDelimiter(delimeter) {}

                    std::string toString() override;
                };
            
                std::vector<base*> parse(std::string_view input);
            }

            namespace sequences {

                enum class specialTypes {
                    NORMAL,
                    HAS_INFINITE_PARAMETERS
                };

                template<
                    typename codeType,
                    typename parameterType              = sequence::parameter::numeric,
                    std::size_t paramCount              = 0,
                    std::size_t intermediateCount       = 0,
                    specialTypes parameterExtension     = specialTypes::NORMAL
                >
                class base {
                public:
                    codeType function;
                    std::array<parameterType, paramCount> parameterDefaultValue;
                    std::array<uint8_t, intermediateCount> intermediateDefaultValues;
                    const specialTypes parameterExtensionType = parameterExtension;

                    base(
                        codeType code,
                        std::array<parameterType, paramCount> defaultParamValues = {},
                        std::array<uint8_t, intermediateCount> defaultIntermediates = {}
                    ) : function(code), parameterDefaultValue(defaultParamValues), intermediateDefaultValues(defaultIntermediates) {}
                };

                namespace delimiters {
                    /**
                     * @brief APC is used as the opening delimiter of a control string for application program use. The command
                        string following may consist of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14. The
                        control string is closed by the terminating delimiter STRING TERMINATOR (ST). The interpretation of
                        the command string depends on the relevant application program. 
                     * @example `09/15` or `01/11 05/15`
                    */
                    inline base<sequence::basic> APPLICATION_PROGRAM_COMMAND = base<sequence::basic>(sequence::basic(table::C1::APC));

                    /**
                     * @brief CMD is used as the delimiter of a string of data coded according to Standard ECMA-35 and to switch to
                        a general level of control. The use of CMD is not mandatory if the higher level protocol defines means of delimiting the string, 
                        for instance, by specifying the length of the string. 
                     * @example `01/11 06/04`
                     */
                    inline base<sequence::independent> CODING_METHOD_DELIMITER = base<sequence::independent>(sequence::independent(table::independentFunctions::CMD));

                    /**
                     * @brief DCS is used as the opening delimiter of a control string for device control use. The command string
                        following may consist of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14. The control
                        string is closed by the terminating delimiter STRING TERMINATOR (ST).
                        The command string represents either one or more commands for the receiving device, or one or more
                        status reports from the sending device. The purpose and the format of the command string are specified
                        by the most recent occurrence of IDENTIFY DEVICE CONTROL STRING (IDCS), if any, or depend on
                        the sending and/or the receiving device. 
                     * @example `09/00` or `01/11 05/00` 
                     */
                    inline base<sequence::basic> DEVICE_CONTROL_STRING = base<sequence::basic>(sequence::basic(table::C1::DCS));

                    /**
                     * @brief OSC is used as the opening delimiter of a control string for operating system use. The command string
                        following may consist of a sequence of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14.
                        The control string is closed by the terminating delimiter STRING TERMINATOR (ST). The
                        interpretation of the command string depends on the relevant operating system. 
                     * @example `09/13` or `01/11 05/13` 
                     */
                    inline base<sequence::basic> OPERATING_SYSTEM_COMMAND = base<sequence::basic>(sequence::basic(table::C1::OSC));

                    /**
                     * @brief PM is used as the opening delimiter of a control string for privacy message use. The command string
                        following may consist of a sequence of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14.
                        The control string is closed by the terminating delimiter STRING TERMINATOR (ST). The
                        interpretation of the command string depends on the relevant privacy discipline.
                     * @example `09/14` or `01/11 05/14` 
                     */
                    inline base<sequence::basic> PRIVACY_MESSAGE = base<sequence::basic>(sequence::basic(table::C1::PM));

                    /**
                     * @brief SOS is used as the opening delimiter of a control string. The character string following may consist of
                        any bit combination, except those representing SOS or STRING TERMINATOR (ST). The control string
                        is closed by the terminating delimiter STRING TERMINATOR (ST). The interpretation of the character
                        string depends on the application.
                     * @example `09/08` or `01/11 05/08`
                     */
                    inline base<sequence::basic> START_OF_STRING = base<sequence::basic>(sequence::basic(table::C1::SOS));

                    /**
                     * @brief ST is used as the closing delimiter of a control string opened by APPLICATION PROGRAM
                        COMMAND (APC), DEVICE CONTROL STRING (DCS), OPERATING SYSTEM COMMAND
                        (OSC), PRIVACY MESSAGE (PM), or START OF STRING (SOS).
                     * @example `09/12` or `01/11 05/12` 
                     */
                    inline base<sequence::basic> STRING_TERMINATOR = base<sequence::basic>(sequence::basic(table::C1::ST));
                };

                namespace introducers {
                    /**
                     * @brief CSI is used as the first character of a control sequence.
                     * @example `09/11` or `01/11 05/11`
                     */
                    inline base<sequence::basic> CONTROL_SEQUENCE_INTRODUCER = base<sequence::basic>(sequence::basic(table::C1::CSI));

                    /**
                     * @brief ESC is used for code extension purposes. It causes the meanings of a limited number of bit combinations
                        following it in the data stream to be changed. 
                     * @example `01/11`
                     */
                    inline base<sequence::basic> ESCAPE = base<sequence::basic>(sequence::basic(table::C0::ESC));

                    /**
                     * @brief SCI and the bit combination following it are used to represent a control function or a graphic character.
                        The bit combination following SCI must be from 00/08 to 00/13 or 02/00 to 07/14. The use of SCI is
                        reserved for future standardization.
                     * @example `09/10` or `01/11 05/10`
                     */
                    inline base<sequence::basic> SINGLE_CHARACTER_INTRODUCER = base<sequence::basic>(sequence::basic(table::C1::SCI));
                }

                namespace shiftFunctions {
                    /**
                     * @brief LS0 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                     * @example `00/15`
                     */
                    inline base<sequence::basic> LOCKING_SHIFT_ZERO = base<sequence::basic>(sequence::basic(table::C0::LS0));

                    /**
                     * @brief LS1 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                     * @example `00/14`
                     */
                    inline base<sequence::basic> LOCKING_SHIFT_ONE = base<sequence::basic>(sequence::basic(table::C0::LS1));

                    /**
                     * @brief LS1R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `07/14`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_ONE_RIGHT = base<sequence::independent>(sequence::independent(table::independentFunctions::LS1R));

                    /**
                     * @brief LS2 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 06/14`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_TWO = base<sequence::independent>(sequence::independent(table::independentFunctions::LS2));

                    /**
                     * @brief LS2R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                    * @example `01/11 07/13`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_TWO_RIGHT = base<sequence::independent>(sequence::independent(table::independentFunctions::LS2R));

                    /**
                     * @brief LS3 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 06/15`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_THREE = base<sequence::independent>(sequence::independent(table::independentFunctions::LS3));

                    /**
                     * @brief LS3R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 07/12`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_THREE_RIGHT = base<sequence::independent>(sequence::independent(table::independentFunctions::LS3R));

                    /**
                     * @brief SI is used for code extension purposes. It causes the meanings of the bit combinations following it in the
                        data stream to be changed. 
                     * @example `00/15`
                     */
                    inline base<sequence::basic> SHIFT_IN = base<sequence::basic>(sequence::basic(table::C0::SI));

                    /**
                     * @brief SO is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `00/14`
                     */
                    inline base<sequence::basic> SHIFT_OUT = base<sequence::basic>(sequence::basic(table::C0::SO));

                    /**
                     * @brief SS2 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `08/14` or `01/11 04/14`
                     */
                    inline base<sequence::basic> SS2 = base<sequence::basic>(sequence::basic(table::C1::SS2));

                    /**
                     * @brief SS3 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `08/15` or `01/11 04/15` 
                     */
                    inline base<sequence::basic> SS3 = base<sequence::basic>(sequence::basic(table::C1::SS3));

                }

                namespace formatEffectors {
                    /**
                     * @brief BS causes the active data position to be moved one character position in the data component in the
                        direction opposite to that of the implicit movement.
                        The direction of the implicit movement depends on the parameter value of SELECT IMPLICIT
                        MOVEMENT DIRECTION (SIMD). 
                     * @example `00/08`
                     */
                    inline base<sequence::basic> BACKSPACE = base<sequence::basic>(sequence::basic(table::C0::BS));

                    /**
                     * @brief The effect of CR depends on the setting of the DEVICE COMPONENT SELECT MODE (DCSM) and
                        on the parameter value of SELECT IMPLICIT MOVEMENT DIRECTION (SIMD).
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION and with the
                        parameter value of SIMD equal to 0, CR causes the active presentation position to be moved to the line
                        home position of the same line in the presentation component. The line home position is established by
                        the parameter value of SET LINE HOME (SLH).
                        With a parameter value of SIMD equal to 1, CR causes the active presentation position to be moved to
                        the line limit position of the same line in the presentation component. The line limit position is
                        established by the parameter value of SET LINE LIMIT (SLL).
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA and with a parameter value of
                        SIMD equal to 0, CR causes the active data position to be moved to the line home position of the same
                        line in the data component. The line home position is established by the parameter value of SET LINE
                        HOME (SLH).
                        With a parameter value of SIMD equal to 1, CR causes the active data position to be moved to the line
                        limit position of the same line in the data component. The line limit position is established by the
                        parameter value of SET LINE LIMIT (SLL).
                     * @example `00/13`
                     */
                    inline base<sequence::basic> CARRIAGE_RETURN = base<sequence::basic>(sequence::basic(table::C0::CR));

                    /**
                     * @brief FF causes the active presentation position to be moved to the corresponding character position of the
                        line at the page home position of the next form or page in the presentation component. The page home
                        position is established by the parameter value of SET PAGE HOME (SPH). 
                     * @example `00/12`
                     */
                    inline base<sequence::basic> FORM_FEED = base<sequence::basic>(sequence::basic(table::C0::FF));

                    /**
                     * @brief HPA causes the active data position to be moved to character position n in the active line (the line in the
                        data component that contains the active data position), where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/00` or `9/11 Pn 06/00`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CHARACTER_POSITION_ABSOLUTE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::HPA), {1});

                    /**
                     * @brief HPB causes the active data position to be moved by n character positions in the data component in the
                        direction opposite to that of the character progression, where n equals the value of Pn.
                     * @example `01/11 05/11 Pn 06/10` or `9/11 Pn 06/10`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CHARACTER_POSITION_BACKWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::HPB), {1});

                    /**
                     * @brief HPR causes the active data position to be moved by n character positions in the data component in the
                        direction of the character progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/01` or `9/11 Pn 06/01`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CHARACTER_POSITION_FORWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::HPR), {1});

                    /**
                     * @brief HT causes the active presentation position to be moved to the following character tabulation stop in the presentation component.
                        In addition, if that following character tabulation stop has been set by TABULATION ALIGN CENTRE
                        (TAC), TABULATION ALIGN LEADING EDGE (TALE), TABULATION ALIGN TRAILING EDGE
                        (TATE) or TABULATION CENTRED ON CHARACTER (TCC), HT indicates the beginning of a string
                        of text which is to be positioned within a line according to the properties of that tabulation stop. The end
                        of the string is indicated by the next occurrence of HT or CARRIAGE RETURN (CR) or NEXT LINE
                        (NEL) in the data stream
                     * @example `00/09`
                     */
                    inline base<sequence::basic> CHARACTER_TABULATION = base<sequence::basic>(sequence::basic(table::C0::HT));

                    /**
                     * @brief HTJ causes the contents of the active field (the field in the presentation component that contains the
                        active presentation position) to be shifted forward so that it ends at the character position preceding the
                        following character tabulation stop. The active presentation position is moved to that following character
                        tabulation stop. The character positions which precede the beginning of the shifted string are put into the
                        erased state. 
                     * @example `08/09` or `01/11 04/09` 
                     */
                    inline base<sequence::basic> CHARACTER_TABULATION_WITH_JUSTIFICATION = base<sequence::basic>(sequence::basic(table::C1::HTJ));

                    /**
                     * @brief HTS causes a character tabulation stop to be set at the active presentation position in the presentation
                        component.
                        The number of lines affected depends on the setting of the TABULATION STOP MODE (TSM). 
                     * @example `08/08` or `01/11 04/08`
                     */
                    inline base<sequence::basic> CHARACTER_TABULATION_SET = base<sequence::basic>(sequence::basic(table::C1::HTS));

                    /**
                     * @brief HVP causes the active data position to be moved in the data component to the n-th line position
                        according to the line progression and to the m-th character position according to the character
                        progression, where n equals the value of Pn1 and m equals the value of Pn2. 
                     * @example `01/11 05/11 Pn1;Pn2 06/06` or `9/11 Pn1;Pn2 06/06`
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2> CHARACTER_AND_LINE_POSITION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::HVP), {1, 1});

                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, LF causes the
                        active presentation position to be moved to the corresponding character position of the following line in
                        the presentation component.
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, LF causes the active data
                        position to be moved to the corresponding character position of the following line in the data
                        component. 
                     * @example `00/10`
                     */
                    inline base<sequence::basic> LINE_FEED = base<sequence::basic>(sequence::basic(table::C0::LF));

                    /**
                     * @brief The effect of NEL depends on the setting of the DEVICE COMPONENT SELECT MODE (DCSM) and
                        on the parameter value of SELECT IMPLICIT MOVEMENT DIRECTION (SIMD).
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION and with a
                        parameter value of SIMD equal to 0, NEL causes the active presentation position to be moved to the line
                        home position of the following line in the presentation component. The line home position is established
                        by the parameter value of SET LINE HOME (SLH).
                        With a parameter value of SIMD equal to 1, NEL causes the active presentation position to be moved to
                        the line limit position of the following line in the presentation component. The line limit position is
                        established by the parameter value of SET LINE LIMIT (SLL).
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA and with a parameter value of
                        SIMD equal to 0, NEL causes the active data position to be moved to the line home position of the
                        following line in the data component. The line home position is established by the parameter value of
                        SET LINE HOME (SLH).
                        With a parameter value of SIMD equal to 1, NEL causes the active data position to be moved to the line
                        limit position of the following line in the data component. The line limit position is established by the
                        parameter value of SET LINE LIMIT (SLL). 
                     * @example `08/05` or `01/11 04/05`
                     */
                    inline base<sequence::basic> NEXT_LINE = base<sequence::basic>(sequence::basic(table::C1::NEL));

                    /**
                     * @brief PLD causes the active presentation position to be moved in the presentation component to the
                        corresponding position of an imaginary line with a partial offset in the direction of the line progression.
                        This offset should be sufficient either to image following characters as subscripts until the first
                        following occurrence of PARTIAL LINE BACKWARD (PLU) in the data stream, or, if preceding
                        characters were imaged as superscripts, to restore imaging of following characters to the active line (the
                        line that contains the active presentation position).
                     * @example `08/11` or `01/11 04/11`
                     */
                    inline base<sequence::basic> PARTIAL_LINE_FORWARD = base<sequence::basic>(sequence::basic(table::C1::PLD));

                    /**
                     * @brief PLU causes the active presentation position to be moved in the presentation component to the
                        corresponding position of an imaginary line with a partial offset in the direction opposite to that of the
                        line progression. This offset should be sufficient either to image following characters as superscripts
                        until the first following occurrence of PARTIAL LINE FORWARD (PLD) in the data stream, or, if
                        preceding characters were imaged as subscripts, to restore imaging of following characters to the active
                        line (the line that contains the active presentation position). 
                     * @example `08/12` or `01/11 04/12` 
                     */
                    inline base<sequence::basic> PARTIAL_LINE_BACKWARD = base<sequence::basic>(sequence::basic(table::C1::PLU));

                    /**
                     * @brief PPA causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/00` or `9/11 Pn 02/00 05/00`
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> PAGE_POSITION_ABSOLUTE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::PPA), {1}, {table::toInt(02, 00)});

                    /**
                     * @brief PPB causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th preceding page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/02` or `9/11 Pn 02/00 05/02`
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> PAGE_POSITION_BACKWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::PPB), {1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief PPR causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th following page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/01` or `9/11 Pn 02/00 05/01`
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> PAGE_POSITION_FORWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::PPR), {1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, RI causes the
                        active presentation position to be moved in the presentation component to the corresponding character
                        position of the preceding line.
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, RI causes the active data
                        position to be moved in the data component to the corresponding character position of the preceding line.
                     @example `08/13` or `ESC 04/13`
                     */
                    inline base<sequence::basic> REVERSE_LINE_FEED = base<sequence::basic>(sequence::basic(table::C1::RI));

                    /**
                     * @brief TBC causes one or more tabulation stops in the presentation component to be cleared, depending on the
                        parameter value. In the case of parameter value 0 or 2 the number of lines affected depends on the setting of the
                        TABULATION STOP MODE (TSM).
                     * @example `01/11 05/11 Ps 06/07` or `9/11 Ps 06/07`
                     */
                    namespace TABULATION_CLEAR {
                        enum class types {
                            CHARACTER_TABULATOR_IN_ACTIVE_POSITION,
                            LINE_TABULATOR_IN_ACTIVE_LINE,
                            ALL_CHARACTER_TABULATORS_IN_ACTIVE_LINE,
                            ALL_CHARACTER_TABULATORS,
                            ALL_LINE_TABULATORS,
                            ALL_LINE_AND_CHARACTER_TABULATORS
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::TBC), {types::CHARACTER_TABULATOR_IN_ACTIVE_POSITION});
                    }

                    /**
                     * @brief TSR causes any character tabulation stop at character position n in the active line (the line that contains
                        the active presentation position) and lines of subsequent text in the presentation component to be
                        cleared, but does not affect other tabulation stops. n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 06/04` or `9/11 Pn 02/00 06/04`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> TABULATION_STOP_REMOVE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TSR), {-1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief VPA causes the active data position to be moved to line position n in the data component in a direction
                        parallel to the line progression, where n equals the value of Pn.
                     * @example `01/11 05/11 Pn 06/04` or `9/11 Pn 06/04`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> LINE_POSITION_ABSOLUTE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::VPA), {1});

                    /**
                     * @brief VPB causes the active data position to be moved by n line positions in the data component in a direction
                        opposite to that of the line progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/11` or `9/11 Pn 06/11`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> LINE_POSITION_BACKWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::VPB), {1});

                    /**
                     * @brief VPR causes the active data position to be moved by n line positions in the data component in a direction
                        parallel to the line progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/05` or `9/11 Pn 06/05`
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> LINE_POSITION_FORWARD(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::VPR), {1});

                    /**
                     * @brief VT causes the active presentation position to be moved in the presentation component to the
                        corresponding character position on the line at which the following line tabulation stop is set. 
                     * @example `00/11`
                     */
                    inline base<sequence::basic> LINE_TABULATION = base<sequence::basic>(sequence::basic(table::C0::VT));

                    /**
                     * @brief VTS causes a line tabulation stop to be set at the active line (the line that contains the active presentation position). 
                     * @example `08/10` or `01/11 04/10`
                     */
                    inline base<sequence::basic> LINE_TABULATION_SET = base<sequence::basic>(sequence::basic(table::C1::VTS));
                }

                namespace presentationControlFunctions {
                    /**
                     * @brief BPH is used to indicate a point where a line break may occur when text is formatted. BPH may occur
                        between two graphic characters, either or both of which may be SPACE. 
                     * @example `08/02` or `01/11 04/02`
                     */
                    inline base<sequence::basic> BREAK_PERMITTED_HERE = base<sequence::basic>(sequence::basic(table::C1::BPH));

                    /**
                     * @brief DTA is used to establish the dimensions of the text area for subsequent pages.
                        The established dimensions remain in effect until the next occurrence of DTA in the data stream. 
                     * @example `01/11 05/11 Pn1;Pn2 02/00 05/04` or `9/11 Pn1;Pn2 02/00 05/04`
                     * @param Pn1 default(none)
                     * @param Pn2 default(none)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2, 1> DIMENSION_TEXT_AREA(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::DTA), {-1, -1}, {table::toInt(02, 00)});

                    /**
                     * @brief FNT is used to identify the character font to be selected as primary or alternative font by subsequent
                        occurrences of SELECT GRAPHIC RENDITION (SGR) in the data stream. Ps1 specifies the primary or alternative font concerned.
                     * @example `01/11 05/11 Ps1;Ps2 02/00 04/04` or `9/11 Ps1;Ps2 02/00 04/04`
                     * @param Ps1 default(0)
                     * @param Ps2 default(0)
                     */
                    namespace FONT_SELECTION {
                        enum class alternatives {
                            PRIMARY,
                            FIRST,
                            SECOND,
                            THIRD,
                            FOURTH,
                            FIFTH,
                            SIXTH,
                            SEVENTH,
                            EIGHT,
                            NINTH
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, alternatives, 2, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::FNT), {alternatives::PRIMARY, alternatives::PRIMARY}, {table::toInt(02, 00)});
                    }

                    /**
                     * @brief GCC is used to indicate that two or more graphic characters are to be imaged as one single graphic
                        symbol. GCC with a parameter value of 0 indicates that the following two graphic characters are to be
                        imaged as one single graphic symbol; GCC with a parameter value of 1 and GCC with a parameter value
                        of 2 indicate respectively the beginning and the end of a string of graphic characters which are to be
                        imaged as one single graphic symbol.
                     * @note GCC does not explicitly specify the relative sizes or placements of the component parts of a composite graphic symbol. 
                        In the simplest case, two components may be "half-width" and side-by-side. For example, 
                        in Japanese text a pair of characters may be presented side-by-side, and occupy the space of a normal-size Kanji character. 
                     * @example `01/11 05/11 Ps1 02/00 05/15` or `9/11 Ps1 02/00 05/15`
                     * @param Ps default(0)
                     */
                    namespace GRAPHIC_CHARACTER_COMBINATION {
                        enum class types {
                            DOUBLE_WIDE,    // Expect next two characters to be as one
                            START,          // beginning and the - 
                            END             //                     end of string characters to be images as a single graphic symbol.
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::GCC), {types::DOUBLE_WIDE}, {table::toInt(02, 00)});
                    }

                    /**
                     * @brief GSM is used to modify for subsequent text the height and/or the width of all primary and alternative
                        fonts identified by FONT SELECTION (FNT) and established by GRAPHIC SIZE SELECTION (GSS).
                        The established values remain in effect until the next occurrence of GSM or GSS in the data steam. 
                     * @example `01/11 05/11 Pn1;Pn2 02/00 04/02` or `9/11 Pn1;Pn2 02/00 04/02`
                     * @param Pn1 default(100) specifies the height as a percentage of the height established by GSS
                     * @param Pn2 default(100) specifies the width as a percentage of the width established by GSS 
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2, 1> GRAPHIC_SIZE_MODIFICATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::GSM), {100,100}, {table::toInt(02, 00)});

                    /**
                     * @brief GSS is used to establish for subsequent text the height and the width of all primary and alternative fonts
                        identified by FONT SELECTION (FNT). The established values remain in effect until the next
                        occurrence of GSS in the data stream.
                        Pn specifies the height, the width is implicitly defined by the height.
                        The unit in which the parameter value is expressed is that established by the parameter value of SELECT
                        SIZE UNIT (SSU).
                     * @example `01/11 05/11 Pn 02/00 04/03` or `9/11 Pn 02/00 04/03`
                     * @param Pn default(none) specifies the height, the width is implicitly defined by the height.
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> GRAPHIC_SIZE_SELECTION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::GSS), {-1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief JFY is used to indicate the beginning of a string of graphic characters in the presentation component that
                        are to be justified according to the layout specified by the parameter values. The end of the string to be justified is indicated by the next occurrence of JFY in the data stream. 
                        The line home position is established by the parameter value of SET LINE HOME (SLH). 
                        The line limit position is established by the parameter value of SET LINE LIMIT (SLL).
                     * @example `01/11 05/11 Ps ... 02/00 04/06` or `9/11 Ps ... 02/00 04/06`
                     * @param Ps default(0)
                     * @param ...  adjusted characters
                     */
                    namespace JUSTIFY {
                        enum class types {
                            NO_JUSTIFICATION,
                            WORD_FILL,
                            WORD_SPACE,
                            LETTER_SPACE,
                            HYPHENATION,
                            FLUSH_TO_LINE_HOME_POSITION_MARGIN,
                            CENTER_BETWEEN_LINE_HOME_POSITION_AND_LINE_LIMIT_POSITION_MARGINS,
                            FLUSH_TO_LINE_LIMIT_POSITION_MARGIN,
                            ITALIAN_HYPHENATION
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::JFY), {types::NO_JUSTIFICATION}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief NBH is used to indicate a point where a line break shall not occur when text is formatted. 
                            NBH may occur between two graphic characters either or both of which may be SPACE. 
                     * @example `08/03` or `01/11 04/03`
                     */
                    inline base<sequence::basic> NO_BREAK_HERE = base<sequence::basic>(sequence::basic(table::C1::NBH));

                    /**
                     * @brief PEC is used to establish the spacing and the extent of the graphic characters for subsequent text. 
                        The spacing is specified in the line as multiples of the spacing established by the most recent occurrence of
                        SET CHARACTER SPACING (SCS) or of SELECT CHARACTER SPACING (SHS) or of SPACING
                        INCREMENT (SPI) in the data stream. The extent of the characters is implicitly established by these control functions. 
                        The established spacing and the extent remain in effect until the next occurrence of
                        PEC, of SCS, of SHS or of SPI in the data stream.
                     * @example `01/11 05/11 Ps 02/00 05/10` or `9/11 Ps 02/00 05/10`
                     * @param Ps default(0)
                     */
                    namespace PRESENTATION_EXPAND_OR_CONTRACT {
                        enum class types {
                            NORMAL,             // as specified by SCS, SHS or SP
                            EXPANDED,           // multiplied by a factor not greater than 2
                            CONDENSED           // multiplied by a factor not less than 0,5
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::PEC), {types::NORMAL}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief PFS is used to establish the available area for the imaging of pages of text based on paper size. 
                        The pages are introduced by the subsequent occurrence of FORM FEED (FF) in the data stream.
                        The established image area remains in effect until the next occurrence of PFS in the data stream.
                        The page home position is established by the parameter value of SET PAGE HOME (SPH), 
                        the page limit position is established by the parameter value of SET PAGE LIMIT (SPL). 
                     * @example `01/11 05/11 Ps 02/00 04/10` or `9/11 Ps 02/00 04/10`
                     * @param Ps default(0)
                     */
                    namespace PAGE_FORMAT_SELECTION {
                        enum class format {
                            TALL_BASIC_COMMUNICATION,
                            WIDE_BASIC_COMMUNICATION,
                            TALL_BASIC_A4,
                            WIDE_BASIC_A4,
                            TALL_NORTH_AMERICAN_LETTER,
                            WIDE_NORTH_AMERICAN_LETTER,
                            TALL_EXTENDED_A4,
                            WIDE_EXTENDED_A4,
                            TALL_NORTH_AMERICAN_LEGAL,
                            WIDE_NORTH_AMERICAN_LEGAL,
                            A4_SHORT_LINES,
                            A4_LONG_LINES,
                            B5_SHORT_LINES,
                            B5_LONG_LINES,
                            B4_SHORT_LINES,
                            B4_LONG_LINES
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, format, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::PFS), {format::TALL_BASIC_COMMUNICATION}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief PTX is used to delimit strings of graphic characters that are communicated one after another in the data
                        stream but that are intended to be presented in parallel with one another, usually in adjacent lines. 
                        PTX with a parameter value of 1 indicates the beginning of the string of principal text intended to be
                        presented in parallel with one or more strings of supplementary text.
                        PTX with a parameter value of 2, 3 or 4 indicates the beginning of a string of supplementary text that is
                        intended to be presented in parallel with either a string of principal text or the immediately preceding
                        string of supplementary text, if any; at the same time it indicates the end of the preceding string of
                        principal text or of the immediately preceding string of supplementary text, if any. The end of a string of
                        supplementary text is indicated by a subsequent occurrence of PTX with a parameter value other than 1.
                        PTX with a parameter value of 0 indicates the end of the strings of text intended to be presented in
                        parallel with one another.
                     * @note PTX does not explicitly specify the relative placement of the strings of principal and supplementary
                        parallel texts, or the relative sizes of graphic characters in the strings of parallel text. A string of
                        supplementary text is normally presented in a line adjacent to the line containing the string of principal
                        text, or adjacent to the line containing the immediately preceding string of supplementary text, if any.
                        The first graphic character of the string of principal text and the first graphic character of a string of
                        supplementary text are normally presented in the same position of their respective lines. However, a
                        string of supplementary text longer (when presented) than the associated string of principal text may be
                        centred on that string. In the case of long strings of text, such as paragraphs in different languages, the
                        strings may be presented in successive lines in parallel columns, with their beginnings aligned with one
                        another and the shorter of the paragraphs followed by an appropriate amount of "white space".
                        Japanese phonetic annotation typically consists of a few half-size or smaller Kana characters which
                        indicate the pronunciation or interpretation of one or more Kanji characters and are presented above
                        those Kanji characters if the character path is horizontal, or to the right of them if the character path is
                        vertical.
                        Chinese phonetic annotation typically consists of a few Pinyin characters which indicate the
                        pronunciation of one or more Hanzi characters and are presented above those Hanzi characters.
                        Alternatively, the Pinyin characters may be presented in the same line as the Hanzi characters and
                        following the respective Hanzi characters. The Pinyin characters will then be presented within enclosing
                        pairs of parentheses. 
                     * @example `01/11 05/11 Ps 05/12` or `9/11 Ps 05/12`
                     * @param Ps default(0)
                     */
                    namespace PARALLEL_TEXTS {
                        enum class types {
                            END,
                            BEGINNING_OF_PRINCIPAL_PARALLEL_TEXT,
                            BEGINNING_OF_SUPPLEMENTARY_PARALLEL_TEXT,
                            BEGINNING_OF_SUPPLEMENTARY_JAPANESE_PHONETIC_ANNOTATION,
                            BEGINNING_OF_SUPPLEMENTARY_CHINESE_PHONETIC_ANNOTATION
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::PTX), {types::END});
                    }

                    /**
                     * @brief QUAD is used to indicate the end of a string of graphic characters that are to be positioned on a single
                        line according to the layout specified by the parameter values.
                        The beginning of the string to be positioned is indicated by the preceding occurrence in the data stream
                        of either QUAD or one of the following formater functions: FORM FEED (FF), CHARACTER AND
                        LINE POSITION (HVP), LINE FEED (LF), NEXT LINE (NEL), PAGE POSITION ABSOLUTE (PPA),
                        PAGE POSITION BACKWARD (PPB), PAGE POSITION FORWARD (PPR), REVERSE LINE FEED
                        (RI), LINE POSITION ABSOLUTE (VPA), LINE POSITION BACKWARD (VPB), LINE POSITION
                        FORWARD (VPR), or LINE TABULATION (VT).
                        The line home position is established by the parameter value of SET LINE HOME (SLH). The line limit
                        position is established by the parameter value of SET LINE LIMIT (SLL). 
                     * @example `01/11 05/11 Ps ... 02/00 04/08` or `9/11 Ps ... 02/00 04/08`
                     * @param Ps default(0)
                     * @param ...
                     */
                    namespace QUAD {
                        enum class types {
                            FLUSH_TO_LINE_HOME_POSITION_MARGIN,
                            FLUSH_TO_LINE_HOME_POSITION_MARGIN_AND_FILL_WITH_HEADER,
                            CENTRE_BETWEEN_LINE_HOME_POSITION_AND_LINE_LIMIT_POSITION_MARGINS,
                            CENTRE_BETWEEN_LINE_HOME_POSITION_AND_LINE_LIMIT_POSITION_MARGINS_AND_FILL_WITH_HEADER,
                            FLUSH_TO_LINE_LIMIT_POSITION_MARGIN,
                            FLUSH_TO_LINE_LIMIT_POSITION_MARGIN_AND_FILL_WITH_HEADER,
                            FLUSH_TO_BOTH_MARGINS
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::QUAD), {types::FLUSH_TO_LINE_HOME_POSITION_MARGIN}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SACS is used to establish extra inter-character escapement for subsequent text. The established extra
                        escapement remains in effect until the next occurrence of SACS or of SET REDUCED CHARACTER
                        SEPARATION (SRCS) in the data stream or until it is reset to the default value by a subsequent
                        occurrence of CARRIAGE RETURN/LINE FEED (CR LF) or of NEXT LINE (NEL) in the data stream
                        The unit in which the parameter value is expressed is that established by the parameter value of SELECT SIZE UNIT (SSU). 
                     * @example `01/11 05/11 Pn 02/00 05/12` or `9/11 Pn 02/00 05/12`
                     * @param Pn default(0)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_ADDITIONAL_CHARACTER_SEPARATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SACS), {0}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SAPV is used to specify one or more variants for the presentation of subsequent text.
                     * @example `01/11 05/11 Ps... 02/00 05/13` or `9/11 Ps... 02/00 05/13`
                     * @param Ps default(0)
                     * @param ...
                     */
                    namespace SELECT_ALTERNATIVE_PRESENTATION_VARIANTS {
                        enum class types {
                            DEFAULT,                                        // default presentation (implementation-defined); cancels the effect of any preceding occurrence of SAPV in the data stream
                            LATIN_DECIMAL_DIGITS,                           // the decimal digits are presented by means of the graphic symbols used in the Latin script
                            ARABIC_DECIMAL_DIGITS,                          // the decimal digits are presented by means of the graphic symbols used in the Arabic script, i.e. the Hindi symbols
                            MIRROR_PAIRED_CHARACTERS,                       // when the direction of the character path is right-to-left, each of the graphic characters in the graphic character set(s) in use which is one of a left/right-handed pair (parentheses, square brackets, curly brackets, greater-than/less-than signs, etc.) is presented as "mirrored", i.e. as the other member of the pair. For example, the coded graphic character given the name LEFT PARENTHESIS is presented as RIGHT PARENTHESIS, and vice versa
                            MIRROR_OPERATORS_AND_DELIMITERS,                // when the direction of the character path is right-to-left, all graphic characters which represent operators and delimiters in mathematical formulae and which are not symmetrical about a vertical axis are presented as mirrored about that vertical axis
                            ISOLATED_FORM,                                  // the following graphic character is presented in its isolated form
                            INITIAL_FORM,                                   // the following graphic character is presented in its initial form
                            MEDIAL_FORM,                                    // the following graphic character is presented in its medial form
                            FINAL_FORM,                                     // the following graphic character is presented in its final form
                            DECIMAL_MARK_FULL_STOP,                         // where the bit combination 02/14 is intended to represent a decimal mark in a decimal number it shall be presented by means of the graphic symbol FULL STOP
                            DECIMAL_MARK_COMMA,                             // where the bit combination 02/14 is intended to represent a decimal mark in a decimal number it shall be presented by means of the graphic symbol COMMA
                            VOWELS_ABOVE_OR_BELOW,                          // vowels are presented above or below the preceding character
                            VOWELS_AFTER,                                   // vowels are presented after the preceding character
                            ARABIC_CONTEXTUAL_WITH_LAM_ALEPH,               // contextual shape determination of Arabic scripts, including the LAM-ALEPH ligature but excluding all other Arabic ligatures
                            ARABIC_CONTEXTUAL_NO_LIGATURES,                 // contextual shape determination of Arabic scripts, excluding all Arabic ligatures
                            CANCEL_MIRRORING,                               // cancels the effect of parameter values 3 and 4
                            VOWELS_NOT_PRESENTED,                           // vowels are not presented
                            CONTEXTUAL_ITALIC_SLANT,                        // when the string direction is right-to-left, the italicized characters are slanted to the left; when the string direction is left-to-right, the italicized characters are slanted to the right
                            ARABIC_PASSTHROUGH_WITH_DIGITS,                 // contextual shape determination of Arabic scripts is not used, the graphic characters - including the digits - are presented in the form they are stored (Pass-through)
                            ARABIC_PASSTHROUGH_WITHOUT_DIGITS,              // contextual shape determination of Arabic scripts is not used, the graphic characters- excluding the digits - are presented in the form they are stored (Pass-through)
                            DECIMAL_DIGITS_DEVICE_DEPENDENT,                // the graphic symbols used to present the decimal digits are device dependent
                            ENABLE_PERSISTENT_FORM_MODE,                    // establishes the effect of parameter values 5, 6, 7, and 8 for the following graphic characters until cancelled
                            CANCEL_PERSISTENT_FORM_MODE                     // cancels the effect of parameter value 21, i.e. re-establishes the effect of parameter values 5, 6, 7, and 8 for the next single graphic character only
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SAPV), {types::DEFAULT}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SCO is used to establish the amount of rotation of the graphic characters following in the data stream. 
                     * The established value remains in effect until the next occurrence of SCO in the data stream.
                     * NOTE: Rotation is positive, i.e. counter-clockwise and applies to the normal presentation of the graphic
                     * characters along the character path. The centre of rotation of the affected graphic characters is not defined by this Standard.
                     * @example `01/11 05/11 Ps 02/00 06/05` or `9/11 Ps 02/00 06/05`
                     * @param Ps default(0)
                     */
                    namespace SET_CHARACTER_ORIENTATION {
                        enum class types {
                            DEFAULT,                                        // 0 degrees.
                            ROTATE_45,                                      // 45 degrees.
                            ROTATE_90,                                      // 90 degrees.
                            ROTATE_135,                                     // 135 degrees.
                            ROTATE_180,                                     // 180 degrees.
                            ROTATE_225,                                     // 225 degrees.
                            ROTATE_270,                                     // 270 degrees.
                            ROTATE_315                                      // 315 degrees.
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SCO), {types::DEFAULT}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SCP is used to select the character path, relative to the line orientation, for the active line (the line that
                     * contains the active presentation position) and subsequent lines in the presentation component. It is also
                     * used to update the content of the active line in the presentation component and the content of the active
                     * line (the line that contains the active data position) in the data component. This takes effect immediately.
                     * NOTE: The second parameter may also permit the effect to take place after the next occurrence of CR, NEL or any control
                     * function which initiates an absolute movement of the active presentation position or the active data position.
                     * @example `01/11 05/11 Ps1;Ps2 02/00 06/11` or `9/11 Ps1;Ps2 02/00 06/11`
                     * @param Ps1 default(None)
                     * @param Ps2 default(None)
                     */
                    namespace SELECT_CHARACTER_PATH {
                        enum class types {
                            LEFT_TO_RIGHT       = 1,        // Ps1: In the case of horizontal line orientation
                            TOP_TO_BOTTOM       = 1,        // Ps1: In the case of vertical line orientation

                            RIGHT_TOLEFT        = 2,        // Ps1: In the case of horizontal line orientation
                            BOTTOM_TO_TOP       = 2,        // Ps1: In the case of vertical line orientation


                            BUFFER_TO_DISPLAY   = 1,        /* Ps2: the content of the active line in the presentation component (the line that contains the active presentation position)
                                                                    is updated to correspond to the content of the active line in the data component (the line that contains the active data position) 
                                                                    according to the newly established character path characteristics in the presentation component; 
                                                                    the active data position is moved to the first character position in the active line in the data component, 
                                                                    the active presentation position in the presentation component is updated accordingly */
                            DISPLAY_TO_BUFFER   = 2         /* Ps2: the content of the active line in the data component (the line that contains the active data position) 
                                                                    is updated to correspond to the content of the active line in the presentation component (the line that contains the active presentation position)
                                                                    according to the newly established character path characteristics of the presentation component; 
                                                                    the active presentation position is moved to the first character position in the active line in the presentation component, 
                                                                    the active data position in the data component is updated accordingly.  */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 2, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SCP), {types::LEFT_TO_RIGHT, types::BUFFER_TO_DISPLAY}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SCS is used to establish the character spacing for subsequent text. The established spacing remains in
                     * effect until the next occurrence of SCS, or of SELECT CHARACTER SPACING (SHS) or of SPACING INCREMENT (SPI) in the data stream, see annex C
                     * @example `01/11 05/11 Pn 02/00 06/07` or `9/11 Pn 02/00 06/07`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_CHARACTER_SPACING(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SCS), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SDS is used to establish in the data component the beginning and the end of a string of characters as
                     * well as the direction of the string. This direction may be different from that currently established. 
                     * The indicated string follows the preceding text. The established character progression is not affected.
                     * The beginning of a directed string is indicated by SDS with a parameter value not equal to 0. 
                     * A directed string may contain one or more nested strings. 
                     * These nested strings may be directed strings the beginnings of which are indicated by SDS with a parameter value not equal to 0,
                     * or reversed strings the beginnings of which are indicated by START REVERSED STRING (SRS) with a parameter value of 1.
                     * Every beginning of such a string invokes the next deeper level of nesting.
                     * This Standard does not define the location of the active data position within any such nested string.
                     * The end of a directed string is indicated by SDS with a parameter value of 0. 
                     * Every end of such a string re-establishes the next higher level of nesting (the one in effect prior to the string just ended).
                     * The direction is re-established to that in effect prior to the string just ended. 
                     * The active data position is moved to the character position following the characters of the string just ended. 
                     * NOTE: 1) The effect of receiving a CVT, HT, SCP, SPD or VT control function within an SDS string is not defined by this Standard.
                     * NOTE: 2) The control functions for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SDS string. 
                     * @example `01/11 05/11 Ps 05/13` or `9/11 Ps 05/13`
                     * @param Ps default(0)
                     */
                    namespace START_DIRECTED_STRING {
                        enum class type {
                            END_OF_DIRECTED_STRING,                     // Re-establish the previous direction
                            START_OF_A_DIRECTED_LEFT_TO_RIGHT_STRING,   // Establish the direction left-to-right
                            START_OF_A_DIRECTED_RIGHT_TO_LEFT_STRING,   // Establish the direction right-to-left
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, type, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SDS), {type::END_OF_DIRECTED_STRING});
                    }

                    /**
                     * @brief SGR is used to establish one or more graphic rendition aspects for subsequent text. 
                     * The established aspects remain in effect until the next occurrence of SGR in the data stream, 
                     * depending on the setting of the GRAPHIC RENDITION COMBINATION MODE (GRCM). 
                     * Each graphic rendition aspect is specified by a parameter value.
                     * NOTE: The usable combinations of parameter values are determined by the implementation. 
                     * @example `01/11 05/11 Ps... 06/13` or `9/11 Ps... 06/13`
                     * @param Ps default(0)
                     * @param ...
                     */
                    namespace SELECT_GRAPHIC_RENDITION {
                        enum class types {
                            DEFAULT,                                        // default rendition (implementation-defined), cancels the effect of any preceding occurrence of SGR in the data stream regardless of the setting of the GRAPHIC RENDITION COMBINATION MODE (GRCM)
                            BOLD,                                           // bold or increased intensity
                            FAINT,                                          // faint, decreased intensity or second colour
                            ITALIC,                                         // italicized
                            UNDERLINE,                                      // singly underlined
                            SLOW_BLINK,                                     // slowly blinking (less then 150 per minute)
                            RAPID_BLINK,                                    // rapidly blinking (150 per minute or more)
                            REVERSE_VIDEO,                                  // negative image
                            CONCEAL,                                        // concealed characters
                            CROSSED_OUT,                                    // crossed-out (characters still legible but marked as to be deleted)
                            PRIMARY_FONT,                                   // primary (default) font
                            ALT_FONT_1,                                     // first alternative font
                            ALT_FONT_2,                                     // second alternative font
                            ALT_FONT_3,                                     // third alternative font
                            ALT_FONT_4,                                     // fourth alternative font
                            ALT_FONT_5,                                     // fifth alternative font
                            ALT_FONT_6,                                     // sixth alternative font
                            ALT_FONT_7,                                     // seventh alternative font
                            ALT_FONT_8,                                     // eighth alternative font
                            ALT_FONT_9,                                     // ninth alternative font
                            FRAKTUR,                                        // Fraktur (Gothic)
                            DOUBLY_UNDERLINED,                              // doubly underlined
                            NORMAL_INTENSITY,                               // normal colour or normal intensity (neither bold nor faint)
                            NOT_ITALIC_NOT_FRAKTUR,                         // not italicized, not fraktur
                            NOT_UNDERLINED,                                 // not underlined (neither singly nor doubly)
                            NOT_BLINKING,                                   // steady (not blinking)
                            RESERVED_PROPORTIONAL_SPACING,                  // (reserved for proportional spacing as specified in CCITT Recommendation T.61)
                            POSITIVE_IMAGE,                                 // positive image
                            REVEAL,                                         // revealed characters
                            NOT_CROSSED_OUT,                                // not crossed out
                            FG_BLACK,                                       // black display
                            FG_RED,                                         // red display
                            FG_GREEN,                                       // green display
                            FG_YELLOW,                                      // yellow display
                            FG_BLUE,                                        // blue display
                            FG_MAGENTA,                                     // magenta display
                            FG_CYAN,                                        // cyan display
                            FG_WHITE,                                       // white display
                            RESERVED_FG_COLOR,                              // (reserved for future standardization; intended for setting character foreground colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
                            FG_DEFAULT,                                     // default display colour (implementation-defined)
                            BG_BLACK,                                       // black background
                            BG_RED,                                         // red background
                            BG_GREEN,                                       // green background
                            BG_YELLOW,                                      // yellow background
                            BG_BLUE,                                        // blue background
                            BG_MAGENTA,                                     // magenta background
                            BG_CYAN,                                        // cyan background
                            BG_WHITE,                                       // white background
                            RESERVED_BG_COLOR,                              // (reserved for future standardization; intended for setting character background colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
                            BG_DEFAULT,                                     // default background colour (implementation-defined)
                            RESERVED_CANCEL_PROPORTIONAL_SPACING,           // (reserved for cancelling the effect of the rendering aspect established by parameter value 26)
                            FRAMED,                                         // framed
                            ENCIRCLED,                                      // encircled
                            OVERLINED,                                      // overlined
                            NOT_FRAMED_NOT_ENCIRCLED,                       // not framed, not encircled
                            NOT_OVERLINED,                                  // not overlined
                            RESERVED_56,                                    // (reserved for future standardization)
                            RESERVED_57,                                    // (reserved for future standardization)
                            RESERVED_58,                                    // (reserved for future standardization)
                            RESERVED_59,                                    // (reserved for future standardization)
                            IDEOGRAM_UNDERLINE,                             // ideogram underline or right side line
                            IDEOGRAM_DOUBLE_UNDERLINE,                      // ideogram double underline or double line on the right side
                            IDEOGRAM_OVERLINE,                              // ideogram overline or left side line
                            IDEOGRAM_DOUBLE_OVERLINE,                       // ideogram double overline or double line on the left side
                            IDEOGRAM_STRESS_MARKING,                        // ideogram stress marking
                            IDEOGRAM_ATTRIBUTES_OFF                         // cancels the effect of the rendition aspects established by parameter values 60 to 64
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 0, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SGR), {types::DEFAULT});
                    }

                    /**
                     * @brief SHS is used to establish the character spacing for subsequent text. 
                     * The established spacing remains in effect until the next occurrence of SHS or of SET CHARACTER SPACING (SCS) or of SPACING INCREMENT (SPI) in the data stream.
                     * @example `01/11 05/11 Ps 02/00 04/11` or `9/11 Ps 02/00 04/11`
                     * @param Ps default(0)
                     */
                    namespace SELECT_CHARACTER_SPACING {
                        enum class types {
                            FIT_10_CHARACTERS_PER_25_4_MM,
                            FIT_12_CHARACTERS_PER_25_4_MM,
                            FIT_15_CHARACTERS_PER_25_4_MM,
                            FIT_6_CHARACTERS_PER_25_4_MM,
                            FIT_3_CHARACTERS_PER_25_4_MM,
                            FIT_9_CHARACTERS_PER_50_8_MM,
                            FIT_4_CHARACTERS_PER_24_4_MM
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SHS), {types::FIT_10_CHARACTERS_PER_25_4_MM}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief  SIMD is used to select the direction of implicit movement of the data position relative to the character progression.
                     * The direction selected remains in effect until the next occurrence of SIMD. 
                     * @example `01/11 05/11 Ps 05/14` or `9/11 Ps 05/14`
                     * @param Ps default(0)
                     */
                    namespace SELECT_IMPLICIT_MOVEMENT_DIRECTION {
                        enum class types {
                            DIRECTION_OF_CHARACTER_PROGRESSION,             // The direction of implicit movement is the same as that of the character progression
                            OPPOSITE_DIRECTION_OF_CHARACTER_PROGRESSION     // The direction of implicit movement is opposite to that of the character progression. 
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SIMD), {types::DIRECTION_OF_CHARACTER_PROGRESSION});
                    }

                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE is set to PRESENTATION, 
                     * SLH is used to establish at character position n in the active line (the line that contains the active presentation position) 
                     * and lines of subsequent text in the presentation component the position to which the active presentation position will be moved by subsequent occurrences of:
                     * CARRIAGE RETURN (CR), DELETE LINE (DL), INSERT LINE (IL) or NEXT LINE (NEL) in the data stream; 
                     * where n equals the value of Pn. 
                     * In the case of a device without data component, it is also the position ahead of which no implicit movement of the active presentation position shall occur.
                     * If the DEVICE COMPONENT SELECT MODE is set to DATA, SLH is used to establish at character position n in the active line (the line that contains the active data position) 
                     * and lines of subsequent text in the data component the position to which the active data position will be moved by subsequent
                     * occurrences of CARRIAGE RETURN (CR), DELETE LINE (DL), INSERT LINE (IL) or NEXT LINE (NEL) in the data stream; where n equals the value of Pn.
                     * It is also the position ahead of which no implicit movement of the active data position shall occur.
                     * The established position is called the line home position and remains in effect until the next occurrence of SLH in the data stream. 
                     * @example `01/11 05/11 Pn 02/00 05/05` or `9/11 Pn 02/00 05/05`
                     * @param Pn default(None)
                    */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_LINE_HOME(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SHL), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE is set to PRESENTATION, SLL is used to establish at character position n in the active line (the line that contains the active presentation position) 
                     * and lines of subsequent text in the presentation component the position to which the active presentation position will be moved by subsequent occurrences of CARRIAGE RETURN (CR), or NEXT LINE (NEL) in the data stream if the parameter value of SELECT IMPLICIT MOVEMENT DIRECTION (SIMD) is equal to 1;
                     * where n equals the value of Pn. 
                     * In the case of a device without data component, it is also the position beyond which no implicit movement of the active presentation position shall occur.
                     * If the DEVICE COMPONENT SELECT MODE is set to DATA, SLL is used to establish at character position n in the active line (the line that contains the active data position) 
                     * and lines of subsequent text in the data component the position beyond which no implicit movement of the active data position shall occur.
                     * It is also the position in the data component to which the active data position will be moved by subsequent occurrences of CR or NEL in the data stream, 
                     * if the parameter value of SELECT IMPLICIT MOVEMENT DIRECTION (SIMD) is equal to 1.
                     * The established position is called the line limit position and remains in effect until the next occurrence of SLL in the data stream. 
                     * @example `01/11 05/11 Pn 02/00 05/06` or `9/11 Pn 02/00 05/06`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_LINE_LIMIT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SLL), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SLS is used to establish the line spacing for subsequent text. 
                     * The established spacing remains in effect until the next occurrence of SLS or of SELECT LINE SPACING (SVS) 
                     * or of SPACING INCREMENT (SPI) in the data stream. 
                     * NOTE: The unit in which the parameter value is expressed is that established by the parameter value of SELECT SIZE UNIT (SSU). 
                     * @example `01/11 05/11 Pn 02/00 06/08` or `9/11 Pn 02/00 06/08`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_LINE_SPACING(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SLS), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SPD is used to select the line orientation, the line progression, and the character path in the presentation component.
                     * It is also used to update the content of the presentation component and the content of the data component. 
                     * This takes effect immediately.
                     * Ps1 specifies the line orientation, the line progression and the character path.
                     * Ps2 specifies the effect on the content of the presentation component and the content of the data component.
                     * NOTE: This may also permit the effect to take place after the next occurrence of CR, FF or any control function which initiates an absolute movement of the active presentation position or the active data position. 
                     * @example `01/11 05/11 Ps1;Ps2 02/00 05/03` or `9/11 Ps1;Ps2 02/00 05/03`
                     * @param Ps1 default(0)
                     * @param Ps2 default(0)
                     */
                    namespace SELECT_PRESENTATION_DIRECTIONS {
                        enum class types {
                            HORIZONTAL_TOP_LEFT_TO_BOTTOM_RIGHT = 0,            /*  Ps1:
                                                                                line orientation:   horizontal
                                                                                line progression:   top-to-bottom
                                                                                character path:     left-to-right */ 

                            VERTICAL_TOP_RIGHT_TO_BOTTOM_LEFT = 1,              /*  Ps1:
                                                                                line orientation:   vertical
                                                                                line progression:   right-to-left
                                                                                character path:     top-to-bottom */

                            VERTICAL_TOP_LEFT_TO_BOTTOM_RIGHT = 2,              /*  Ps1:
                                                                                line orientation:   vertical
                                                                                line progression:   left-to-right
                                                                                character path:     top-to-bottom */

                            HORIZONTAL_TOP_RIGHT_TO_BOTTOM_LEFT = 3,            /*  Ps1:
                                                                                line orientation:   horizontal
                                                                                line progression:   top-to-bottom
                                                                                character path:     right-to-left */

                            VERTICAL_BOTTOM_LEFT_TO_TOP_RIGHT = 4,              /*  Ps1:
                                                                                line orientation:   vertical
                                                                                line progression:   left-to-right
                                                                                character path:     bottom-to-top */

                            HORIZONTAL_BOTTOM_RIGHT_TO_TOP_LEFT = 5,            /*  Ps1:
                                                                                line orientation:   horizontal
                                                                                line progression:   bottom-to-top
                                                                                character path:     right-to-left */

                            HORIZONTAL_BOTTOM_LEFT_TO_TOP_RIGHT = 6,            /*  Ps1:
                                                                                line orientation:   horizontal
                                                                                line progression:   bottom-to-top
                                                                                character path:     left-to-right */

                            VERTICAL_BOTTOM_RIGHT_TO_TOP_LEFT = 7,              /*  Ps1:
                                                                                line orientation:   vertical
                                                                                line progression:   right-to-left
                                                                                character path:     bottom-to-top */


                            STALL                           = 0,                // Ps2: Undefined (implementation-dependent) 

                            BUFFER_TO_DISPLAY               = 1,                /* Ps2: The content of the presentation component is updated to correspond to the content of the data
                                                                                        component according to the newly established characteristics of the presentation component; 
                                                                                        the active data position is moved to the first character position in the first line in the data component, 
                                                                                        the active presentation position in the presentation component is updated accordingly */

                            DISPLAY_TO_BUFFER               = 2,                /* Ps2: The content of the data component is updated to correspond to the content of the presentation
                                                                                        component according to the newly established characteristics of the presentation component; 
                                                                                        the active presentation position is moved to the first character position in the first line in the presentation component,
                                                                                        the active data position in the data component is updated accordingly. */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 2, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SPD), {types::HORIZONTAL_TOP_LEFT_TO_BOTTOM_RIGHT, types::STALL}, {table::toInt(2, 00)});
                    }

                    // inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> SET_PAGE_HOME                                 =       sequence::controlSequence<sequence::parameter::numeric>(1, table::finalWithIntermediate::SPH); // Ecma lists these, but there are no mentions in the tables.
                    
                    /**
                     * @brief SPI is used to establish the line spacing and the character spacing for subsequent text. 
                     * The established line spacing remains in effect until the next occurrence of SPI or of SET LINE SPACING (SLS) or of SELECT LINE SPACING (SVS) in the data stream. 
                     * The established character spacing remains in effect until the next occurrence of SET CHARACTER SPACING (SCS) or of SELECT CHARACTER SPACING (SHS) in the data stream, see annex C. 
                     * NOTE: The unit in which the parameter values are expressed is that established by the parameter value of SELECT SIZE UNIT (SSU). 
                     * @example `01/11 05/11 Pn1;Pn2 02/00 04/07` or `9/11 Pn1;Pn2 02/00 04/07`
                     * @param Pn1 default(None)
                     * @param Pn2 default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2, 1> SPACING_INCREMENT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SPI), {}, {table::toInt(2, 00)});
                    
                    // inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> SET_PAGE_LIMIT                                =       sequence::controlSequence<sequence::parameter::numeric>(1, table::finalWithIntermediate::SPL); // Ecma lists these, but there are no mentions in the tables.
                    
                    /**
                     * @brief SPQR is used to select the relative print quality and the print speed for devices the output quality and speed of which are inversely related.
                     * The selected values remain in effect until the next occurrence of SPQR in the data stream.
                     * @example `01/11 05/11 Ps 02/00 05/08` or `9/11 Ps 02/00 05/08`
                     * @param Ps default(0)
                     */
                    namespace SELECT_PRINT_QUALITY_AND_RAPIDITY {
                        enum class types {
                            SLOW_SPEED,             // Highest quality
                            MEDIUM_SPEED,           // Medium quality
                            FAST_SPEED              // Draft quality
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1>code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SPQR), {types::SLOW_SPEED}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SRCS is used to establish reduced inter-character escapement for subsequent text. 
                     * The established reduced escapement remains in effect until the next occurrence of SRCS or of SET ADDITIONAL CHARACTER SEPARATION (SACS) in the data stream or until it is reset to the default value by a
                     * subsequent occurrence of CARRIAGE RETURN/LINE FEED (CR/LF) or of NEXT LINE (NEL) in the data stream, see annex C.
                     * Pn specifies the number of units by which the inter-character escapement is reduced. 
                     * The unit in which the parameter values are expressed is that established by the parameter value of SELECT SIZE UNIT (SSU). 
                     * @example `01/11 05/11 Pn 02/00 06/06` or `9/11 Pn 02/00 06/06`
                     * @param Pn default(0)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_REDUCED_CHARACTER_SEPARATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SRCS), {0}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SRS is used to establish in the data component the beginning and the end of a string of characters as well
                     * as the direction of the string. This direction is opposite to that currently established. The indicated string
                     * follows the preceding text. The established character progression is not affected.
                     * The beginning of a reversed string is indicated by SRS with a parameter value of 1. A reversed string
                     * may contain one or more nested strings. These nested strings may be reversed strings the beginnings of
                     * which are indicated by SRS with a parameter value of 1, or directed strings the beginnings of which are
                     * indicated by START DIRECTED STRING (SDS) with a parameter value not equal to 0. Every
                     * beginning of such a string invokes the next deeper level of nesting.
                     * This Standard does not define the location of the active data position within any such nested string.
                     * The end of a reversed string is indicated by SRS with a parameter value of 0. Every end of such a string
                     * re-establishes the next higher level of nesting (the one in effect prior to the string just ended). The
                     * direction is re-established to that in effect prior to the string just ended. The active data position is
                     * moved to the character position following the characters of the string just ended.
                     * NOTE 1: The effect of receiving a CVT, HT, SCP, SPD or VT control function within an SRS string is not defined by this Standard.
                     * NOTE 2: The control functions for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SRS string.
                     * @example `01/11 05/11 Ps 05/11` or `9/11 Ps 05/11`
                     * @param Ps default(0)
                     */
                    namespace START_REVERSED_STRING {
                        enum class types {
                            END_OF_REVERSED_STRING,                     // end of a reversed string; re-establish the previous direction
                            START_OF_REVERSED_STRING                    // beginning of a reversed string; reverse the direction
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SRS), {types::END_OF_REVERSED_STRING});
                    }

                    /**
                     * @brief SSU is used to establish the unit in which the numeric parameters of certain control functions are expressed. 
                     * The established unit remains in effect until the next occurrence of SSU in the data stream.
                     * @example `01/11 05/11 Ps 02/00 04/09` or `9/11 Ps 02/00 04/09`
                     * @param Ps default(0)
                     */
                    namespace SELECT_SIZE_UNIT {
                        enum class types {
                            CHARACTER,                          // The dimensions of this unit are device-dependent
                            MILLIMETRE,                         // 1 mm
                            COMPUTER_DECIPOINT,                 // 0,035 28 mm (1/720 of 25,4 mm)
                            DECIDIDOT,                          // 0,037 59 mm (10/266 mm)
                            MIL,                                // 0,025 4 mm (1/1 000 of 25,4 mm)
                            BASIC_MEASURING_UNIT,               // (BMU) - 0,021 17 mm (1/1 200 of 25,4 mm)
                            MICROMETRE,                         // 0,001 mm
                            PIXEL,                              // The smallest increment that can be specified in a device
                            DECIPOINT                           // 0,035 14 mm (35/996 mm)
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SSU), {types::CHARACTER}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief SSW is used to establish for subsequent text the character escapement associated with the character SPACE. 
                     * The established escapement remains in effect until the next occurrence of SSW in the data stream or until it is reset to the default value by a subsequent 
                     * occurrence of CARRIAGE RETURN/LINE FEED (CR/LF), CARRIAGE RETURN/FORM FEED (CR/FF), or of NEXT LINE (NEL) in the data stream, see annex C.
                     * Pn specifies the escapement.
                     * The unit in which the parameter value is expressed is that established by the parameter value of SELECT SIZE UNIT (SSU).
                     * The default character escapement of SPACE is specified by the most recent occurrence of SET CHARACTER SPACING (SCS) or of SELECT CHARACTER SPACING (SHS) 
                     * or of SELECT SPACING INCREMENT (SPI) in the data stream if the current font has constant spacing, or is specified by the nominal width of the character SPACE 
                     * in the current font if that font has proportional spacing.
                     * @example `01/11 05/11 Pn 02/00 05/11` or `9/11 Pn 02/00 05/11`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SET_SPACE_WIDTH(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SSW), {}, {table::toInt(2, 00)});

                    /**
                     * @brief STAB causes subsequent text in the presentation component to be aligned according to the position and
                     * the properties of a tabulation stop which is selected from a list according to the value of the parameter Ps.
                     * The use of this control function and means of specifying a list of tabulation stops to be referenced by the
                     * control function are specified in other standards, for example ISO 8613-6.
                     * @example `01/11 05/11 Ps 02/00 05/14` or `9/11 Ps 02/00 05/14`
                     * @param Ps default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SELECTIVE_TABULATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::STAB), {}, {table::toInt(2, 00)});

                    /**
                     * @brief SVS is used to establish the line spacing for subsequent text. The established spacing remains in effect
                     * until the next occurrence of SVS or of SET LINE SPACING (SLS) or of SPACING INCREMENT (SPI) in the data stream.
                     * @example `01/11 05/11 Ps 02/00 04/12` or `9/11 Ps 02/00 04/12`
                     * @param Ps default(0)
                     */
                    namespace SELECT_LINE_SPACING {
                        enum class types {
                            SIX_LINES_PER_25_4_MM,      // 6 lines per 25,4 mm
                            FOUR_LINES_PER_25_4_MM,     // 4 lines per 25,4 mm
                            THREE_LINES_PER_25_4_MM,    // 3 lines per 25,4 mm
                            TWELVE_LINES_PER_25_4_MM,   // 12 lines per 25,4 mm
                            EIGHT_LINES_PER_25_4_MM,    // 8 lines per 25,4 mm
                            SIX_LINES_PER_30_0_MM,      // 6 lines per 30,0 mm
                            FOUR_LINES_PER_30_0_MM,     // 4 lines per 30,0 mm
                            THREE_LINES_PER_30_0_MM,    // 3 lines per 30,0 mm
                            TWELVE_LINES_PER_30_0_MM,   // 12 lines per 30,0 mm
                            TWO_LINES_PER_25_4_MM       // 2 lines per 25,4 mm
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::SVS), {types::SIX_LINES_PER_25_4_MM}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief TAC causes a character tabulation stop calling for centring to be set at character position n in the active line (the line that contains the active presentation position) and lines of subsequent text in the presentation component, where n equals the value of Pn. 
                     * TAC causes the replacement of any tabulation stop previously set at that character position, but does not affect other tabulation stops.
                     * A text string centred upon a tabulation stop set by TAC will be positioned so that the (trailing edge of the) first graphic character and the (leading edge of the) last graphic character are at approximately equal distances from the tabulation stop. 
                     * @example `01/11 05/11 Pn 02/00 06/02` or `9/11 Pn 02/00 06/02`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> TABULATION_ALIGNED_CENTRED(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TAC), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief TALE causes a character tabulation stop calling for leading edge alignment to be set at character position n in the active line (the line that contains the active presentation position) and lines of subsequent text in the presentation component, where n equals the value of Pn.
                     * TALE causes the replacement of any tabulation stop previously set at that character position, but does not affect other tabulation stops.
                     * A text string aligned with a tabulation stop set by TALE will be positioned so that the (leading edge of the) last graphic character of the string is placed at the tabulation stop. 
                     * @example `01/11 05/11 Pn 02/00 06/01` or `9/11 Pn 02/00 06/01`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> TABULATION_ALIGNED_LEADING_EDGE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TALE), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief TATE causes a character tabulation stop calling for trailing edge alignment to be set at character
                     * position n in the active line (the line that contains the active presentation position) and lines of
                     * subsequent text in the presentation component, where n equals the value of Pn. 
                     * TATE causes the replacement of any tabulation stop previously set at that character position, but does not affect other tabulation stops.
                     * A text string aligned with a tabulation stop set by TATE will be positioned so that the (trailing edge of the) first graphic character of the string is placed at the tabulation stop. 
                     * @example `01/11 05/11 Pn 02/00 06/00` or `9/11 Pn 02/00 06/00`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> TABULATION_ALIGNED_TRAILING_EDGE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TATE), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief TCC causes a character tabulation stop calling for alignment of a target graphic character to be set at
                     * character position n in the active line (the line that contains the active presentation position) and lines of
                     * subsequent text in the presentation component, where n equals the value of Pn1, and the target character
                     * about which centring is to be performed is specified by Pn2. TCC causes the replacement of any
                     * tabulation stop previously set at that character position, but does not affect other tabulation stops.
                     * The positioning of a text string aligned with a tabulation stop set by TCC will be determined by the first
                     * occurrence in the string of the target graphic character; that character will be centred upon the tabulation
                     * stop. If the target character does not occur within the string, then the trailing edge of the first character
                     * of the string will be positioned at the tabulation stop.
                     * The value of Pn2 indicates the code table position (binary value) of the target character in the currently
                     * invoked code. For a 7-bit code, the permissible range of values is 32 to 127; for an 8-bit code, the
                     * permissible range of values is 32 to 127 and 160 to 255. 
                     * @example `01/11 05/11 Pn1;Pn2 02/00 06/03` or `9/11 Pn1;Pn2 02/00 06/03`
                     * @param Pn1 default(None)
                     * @param Pn2 default(32)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2, 1> TABULATION_CENTRED_ON_CHARACTER(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TCC), {0, 32}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief TSS is used to establish the width of a thin space for subsequent text. 
                     * The established width remains in effect until the next occurrence of TSS in the data stream, see annex C.
                     * Pn specifies the width of the thin space. 
                     * The unit in which the parameter value is expressed is that established by the parameter value of SELECT SIZE UNIT (SSU). 
                     * @example `01/11 05/11 Pn 02/00 04/05` or `9/11 Pn 02/00 04/05`
                     * @param Pn default(None) 
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> THIN_SPACE_SPECIFICATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::TSS), {}, {table::toInt(2, 00)});
                }

                namespace editorFunctions {
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, DCH causes the
                     * contents of the active presentation position and, depending on the setting of the CHARACTER
                     * EDITING MODE (HEM), the contents of the n-1 preceding or following character positions to be
                     * removed from the presentation component, where n equals the value of Pn. The resulting gap is closed
                     * by shifting the contents of the adjacent character positions towards the active presentation position. At
                     * the other end of the shifted part, n character positions are put into the erased state.
                     * The extent of the shifted part is established by SELECT EDITING EXTENT (SEE).
                     * The effect of DCH on the start or end of a selected area, the start or end of a qualified area, or a
                     * tabulation stop in the shifted part is not defined by this Standard.
                     * If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, DCH causes the contents of
                     * the active data position and, depending on the setting of the CHARACTER EDITING MODE (HEM),
                     * the contents of the n-1 preceding or following character positions to be removed from the data
                     * component, where n equals the value of Pn. The resulting gap is closed by shifting the contents of the
                     * adjacent character positions towards the active data position. At the other end of the shifted part, n
                     * character positions are put into the erased state. 
                     * @example `01/11 05/11 Pn 05/00` or `9/11 Pn 05/00`
                     * @param Pn default(1) 
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> DELETE_CHARACTER(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::DCH), {1});
                    
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, DL causes the
                        contents of the active line (the line that contains the active presentation position) and, depending on the
                        setting of the LINE EDITING MODE (VEM), the contents of the n-1 preceding or following lines to be
                        removed from the presentation component, where n equals the value of Pn. 
                        The resulting gap is closed by shifting the contents of a number of adjacent lines towards the active line. 
                        At the other end of the shifted part, n lines are put into the erased state.
                        The active presentation position is moved to the line home position in the active line. The line home
                        position is established by the parameter value of SET LINE HOME (SLH). If the TABULATION STOP
                        MODE (TSM) is set to SINGLE, character tabulation stops are cleared in the lines that are put into the erased state.
                        The extent of the shifted part is established by SELECT EDITING EXTENT (SEE).
                        Any occurrences of the start or end of a selected area, the start or end of a qualified area, or a tabulation stop in the shifted part, are also shifted.
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, DL causes the contents of the
                        active line (the line that contains the active data position) and, depending on the setting of the LINE
                        EDITING MODE (VEM), the contents of the n-1 preceding or following lines to be removed from the
                        data component, where n equals the value of Pn. 
                        The resulting gap is closed by shifting the contents of a number of adjacent lines towards the active line. 
                        At the other end of the shifted part, n lines are put into the erased state. 
                        The active data position is moved to the line home position in the active line. 
                        The line home position is established by the parameter value of SET LINE HOME (SLH). 
                     * @example `01/11 05/11 Pn 04/13` or `9/11 Pn 04/13`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> DELETE_LINE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::DL), {1});

                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, EA causes some or
                     * all character positions in the active qualified area (the qualified area in the presentation component
                     * which contains the active presentation position) to be put into the erased state, depending on the parameter values
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, EA causes some or all
                     * character positions in the active qualified area (the qualified area in the data component which contains the active data position) 
                     * to be put into the erased state, depending on the parameter values
                     * NOTE: Whether the character positions of protected areas are put into the erased state, or the character positions
                     * of unprotected areas only, depends on the setting of the ERASURE MODE (ERM). 
                     * @example `01/11 05/11 Pn 04/15` or `9/11 Pn 04/15`
                     * @param Ps default(0)
                     */
                    namespace ERASE_IN_AREA {
                        enum class types {
                            FROM_ACTIVE_POSITION_UNTIL_QUALIFIED_AREA_END,          /* 1) The active presentation position and the character positions up to the end of the qualified area are put into the erased state 
                                                                                       2) The active data position and the character positions up to the end of the qualified area are put into the erased state  */
                        
                            FROM_QUALIFIED_AREA_START_UNTIL_ACTIVE_POSITION,        /* 1) The character positions from the beginning of the qualified area up to and including the active presentation position are put into the erased state 
                                                                                       2) The character positions from the beginning of the qualified area up to and including the active data position are put into the erased state   */

                            ALL_OF_QUALIFIED_AREA,                                  /* 1) All character positions of the qualified area are put into the erased state
                                                                                       2) All character positions in the qualified area are put into the erased state  */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::EA), {types::FROM_ACTIVE_POSITION_UNTIL_QUALIFIED_AREA_END});
                    }

                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, ECH causes the
                     * active presentation position and the n-1 following character positions in the presentation component to
                     * be put into the erased state, where n equals the value of Pn.
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, ECH causes the active data
                     * position and the n-1 following character positions in the data component to be put into the erased state, where n equals the value of Pn.
                     * NOTE: Whether the character positions of protected areas are put into the erased state, or the character positions
                     * of unprotected areas only, depends on the setting of the ERASURE MODE (ERM). 
                     * @example `01/11 05/11 Pn 05/08` or `9/11 Pn 05/08`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> ERASE_CHARACTER(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::ECH), {1});
                    
                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, ED causes some or
                     * all character positions of the active page (the page which contains the active presentation position in the presentation component) to be put into the erased state, 
                     * depending on the parameter values.
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, ED causes some or all
                     * character positions of the active page (the page which contains the active data position in the data component)
                     * to be put into the erased state, depending on the parameter values.
                     * NOTE: Whether the character positions of protected areas are put into the erased state, or the character positions
                     * of unprotected areas only, depends on the setting of the ERASURE MODE (ERM). 
                     * @example `01/11 05/11 Ps 04/10` or `9/11 Ps 04/10`
                     * @param Ps default(0)
                     */
                    namespace ERASE_IN_PAGE {
                        enum class types {
                            FROM_ACTIVE_POSITION_UNTIL_END_OF_PAGE,                 /* 1) The active presentation position and the character positions up to the end of the page are put into the erased state 
                                                                                       2) The active data position and the character positions up to the end of the page are put into the erased state  */
                        
                            FROM_PAGE_START_UNTIL_ACTIVE_POSITION,                  /* 1) The character positions from the beginning of the page up to and including the active presentation position are put into the erased state 
                                                                                       2) The character positions from the beginning of the page up to and including the active data position are put into the erased state    */

                            WHOLE_PAGE,                                             /* 1) All character positions of the page are put into the erased state 
                                                                                       2) All character positions of the page are put into the erased state  */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::ED), {types::FROM_ACTIVE_POSITION_UNTIL_END_OF_PAGE});
                    }

                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, EF causes some or
                     * all character positions of the active field (the field which contains the active presentation position in the presentation component)
                     * to be put into the erased state, depending on the parameter values.
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, EF causes some or all
                     * character positions of the active field (the field which contains the active data position in the data component) 
                     * to be put into the erased state, depending on the parameter values.
                     * NOTE: Whether the character positions of protected areas are put into the erased state, or the character positions of unprotected areas only, depends on the setting of the ERASURE MODE (ERM). 
                     * @example `01/11 05/11 Ps 04/14` or `9/11 Ps 04/14`
                     * @param Ps default(0)
                     */
                    namespace ERASE_IN_FIELD {
                        enum class types {
                            FROM_ACTIVE_POSITION_UNTIL_END_OF_FIELD,                 /* 1) The active presentation position and the character positions up to the end of the field are put into the erased state 
                                                                                        2) The active data position and the character positions up to the end of the field are put into the erased state  */
                        
                            FROM_FIELD_START_UNTIL_ACTIVE_POSITION,                  /* 1) The character positions from the beginning of the field up to and including the active presentation position are put into the erased state
                                                                                        2) The character positions from the beginning of the field up to and including the active data position are put into the erased state     */

                            WHOLE_FIELD,                                             /* 1) All character positions of the field are put into the erased state
                                                                                        2) All character positions of the field are put into the erased state  */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::EF), {types::FROM_ACTIVE_POSITION_UNTIL_END_OF_FIELD});
                    }

                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, EL causes some or
                     * all character positions of the active line (the line which contains the active presentation position in the presentation component) to be put into the erased state, 
                     * depending on the parameter values.
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, EL causes some or all
                     * character positions of the active line (the line which contains the active data position in the data component) to be put into the erased state, 
                     * depending on the parameter values.
                     * NOTE: Whether the character positions of protected areas are put into the erased state, or the character positions of unprotected areas only, depends on the setting of the ERASURE MODE (ERM). 
                     * @example `01/11 05/11 Ps 04/11` or `9/11 Ps 04/11`
                     * @param Ps default(0)
                     */
                    namespace ERASE_IN_LINE {
                        enum class types {
                            FROM_ACTIVE_POSITION_UNTIL_END_OF_LINE,              /* 1) The active presentation position and the character positions up to the end of the line are put into the erased state 
                                                                                    2) The active data position and the character positions up to the end of the line are put into the erased state  */
                        
                            FROM_LINE_START_UNTIL_ACTIVE_POSITION,               /* 1) The character positions from the beginning of the line up to and including the active presentation position are put into the erased state
                                                                                    2) The character positions from the beginning of the line up to and including the active data position are put into the erased state     */

                            WHOLE_LINE,                                          /* 1) All character positions of the line are put into the erased state
                                                                                    2) All character positions of the line are put into the erased state  */
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::EL), {types::FROM_ACTIVE_POSITION_UNTIL_END_OF_LINE});
                    }

                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, ICH is used to
                     * prepare the insertion of n characters, by putting into the erased state the active presentation position and,
                     * depending on the setting of the CHARACTER EDITING MODE (HEM), the n-1 preceding or following
                     * character positions in the presentation component, where n equals the value of Pn. 
                     * The previous contents of the active presentation position and an adjacent string of character positions are shifted away from the
                     * active presentation position. The contents of n character positions at the other end of the shifted part are removed. 
                     * The active presentation position is moved to the line home position in the active line. 
                     * The line home position is established by the parameter value of SET LINE HOME (SLH).
                     * The extent of the shifted part is established by SELECT EDITING EXTENT (SEE).
                     * The effect of ICH on the start or end of a selected area, the start or end of a qualified area, or a
                     * tabulation stop in the shifted part, is not defined by this Standard.
                     * If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, ICH is used to prepare the
                     * insertion of n characters, by putting into the erased state the active data position and, depending on the
                     * setting of the CHARACTER EDITING MODE (HEM), the n-1 preceding or following character
                     * positions in the data component, where n equals the value of Pn. The previous contents of the active data
                     * position and an adjacent string of character positions are shifted away from the active data position. 
                     * The contents of n character positions at the other end of the shifted part are removed. 
                     * The active data position is moved to the line home position in the active line. 
                     * The line home position is established by the parameter value of SET LINE HOME (SLH). 
                     * @example `01/11 05/11 Pn 04/00` or `9/11 Pn 04/00`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> INSERT_CHARACTER(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::ICH), {1});
                    
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, IL is used to
                     * prepare the insertion of n lines, by putting into the erased state in the presentation component the active
                     * line (the line that contains the active presentation position) and, depending on the setting of the LINE
                     * EDITING MODE (VEM), the n-1 preceding or following lines, where n equals the value of Pn. 
                     * The previous contents of the active line and of adjacent lines are shifted away from the active line. 
                     * The contents of n lines at the other end of the shifted part are removed. The active presentation position is
                     * moved to the line home position in the active line. The line home position is established by the
                     * parameter value of SET LINE HOME (SLH).
                     * The extent of the shifted part is established by SELECT EDITING EXTENT (SEE).
                     * Any occurrences of the start or end of a selected area, the start or end of a qualified area, or a tabulation
                     * stop in the shifted part, are also shifted.
                     * If the TABULATION STOP MODE (TSM) is set to SINGLE, character tabulation stops are cleared in
                     * the lines that are put into the erased state.
                     * If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, IL is used to prepare the
                     * insertion of n lines, by putting into the erased state in the data component the active line (the line that
                     * contains the active data position) and, depending on the setting of the LINE EDITING MODE (VEM),
                     * the n-1 preceding or following lines, where n equals the value of Pn. 
                     * The previous contents of the active line and of adjacent lines are shifted away from the active line. The contents of n lines at the other end
                     * of the shifted part are removed. 
                     * The active data position is moved to the line home position in the active line.
                     * The line home position is established by the parameter value of SET LINE HOME (SLH). 
                     * @example `01/11 05/11 Pn 04/12` or `9/11 Pn 04/12`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> INSERT_LINE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::IL), {1});
                }

                namespace cursorControlFunctions {
                    /**
                     * @brief CBT causes the active presentation position to be moved to the character position corresponding to the
                     * n-th preceding character tabulation stop in the presentation component, according to the character path,
                     * where n equals the value of Pn
                     * @example `01/11 05/11 Pn 05/10` or `9/11 Pn 05/10`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_BACKWARD_TABULATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CBT), {1});
                    
                    /**
                     * @brief CHA causes the active presentation position to be moved to character position n in the active line in the
                     * presentation component, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/07` or `9/11 Pn 04/07`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_CHARACTER_ABSOLUTE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CHA), {1});
                    
                    /**
                     * @brief CHT causes the active presentation position to be moved to the character position corresponding to the
                     * n-th following character tabulation stop in the presentation component, according to the character path,
                     * where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/09` or `9/11 Pn 04/09`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_FORWARD_TABULATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CHT), {1});
                    
                    /**
                     * @brief CNL causes the active presentation position to be moved to the first character position of the n-th
                     * following line in the presentation component, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/05` or `9/11 Pn 04/05`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_NEXT_LINE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CNL), {1});
                    
                    /**
                     * @brief CPL causes the active presentation position to be moved to the first character position of the n-th
                     * preceding line in the presentation component, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/06` or `9/11 Pn 04/06`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_PRECEDING_LINE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CPL), {1});
                    
                    /**
                     * @brief CTC causes one or more tabulation stops to be set or cleared in the presentation component, depending on the parameter values.
                     * NOTE: In the case of parameter values 0, 2 or 4 the number of lines affected depends on the setting of the TABULATION STOP MODE (TSM). 
                     * @example `01/11 05/11 Ps... 05/07` or `9/11 Ps... 05/07`
                     * @param Ps default(0)
                     */
                    namespace CURSOR_TABULATION_CONTROL {
                        enum class types {
                            INSERT_CHARACTER_STOP_AT_ACTIVE_POSITION,                       // A character tabulation stop is set at the active presentation position
                            INSERT_LINE_STOP_AT_ACTIVE_POSITION,                            // A line tabulation stop is set at the active line (the line that contains the active presentation position)
                            CLEAR_CHARACTER_STOP_AT_ACTIVE_POSITION,                        // The character tabulation stop at the active presentation position is cleared 
                            CLEAR_LINE_STOP_AT_ACTIVE_POSITION,                             // The line tabulation stop at the active line is cleared 
                            CLEAR_ALL_CHARACTER_STOPS,                                      // All character tabulation stops are cleared 
                            CLEAR_ALL_LINE_STOPS,                                           // All line tabulation stops are cleared
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 0, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>( table::finalWithoutIntermediate::CTC), {types::INSERT_CHARACTER_STOP_AT_ACTIVE_POSITION});
                    }

                    /**
                     * @brief CUB causes the active presentation position to be moved leftwards in the presentation component by n
                     * character positions if the character path is horizontal, or by n line positions if the character path is
                     * vertical, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/04` or `9/11 Pn 04/04`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_LEFT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CUB), {1});
                    
                    /**
                     * @brief CUD causes the active presentation position to be moved downwards in the presentation component by n
                     * line positions if the character path is horizontal, or by n character positions if the character path is
                     * vertical, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/02` or `9/11 Pn 04/02`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_DOWN(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CUD), {1});
                    
                    /**
                     * @brief CUF causes the active presentation position to be moved rightwards in the presentation component by n
                     * character positions if the character path is horizontal, or by n line positions if the character path is
                     * vertical, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/03` or `9/11 Pn 04/03`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_RIGHT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CUF), {1});
                    
                    /**
                     * @brief CUP causes the active presentation position to be moved in the presentation component to the n-th line
                     * position according to the line progression and to the m-th character position according to the character
                     * path, where n equals the value of Pn1 and m equals the value of Pn2. 
                     * @example `01/11 05/11 Pn1;Pn2 04/08` or `9/11 Pn1;Pn2 04/08`
                     * @param Pn1 default(1)
                     * @param Pn2 default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2> CURSOR_POSITION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CUP), {1, 1});
                    
                    /**
                     * @brief CUU causes the active presentation position to be moved upwards in the presentation component by n
                     * line positions if the character path is horizontal, or by n character positions if the character path is
                     * vertical, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 04/01` or `9/11 Pn 04/01`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_UP(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CUU), {1});
                    
                    /**
                     * @brief CVT causes the active presentation position to be moved to the corresponding character position of the
                     * line corresponding to the n-th following line tabulation stop in the presentation component, where n
                     * equals the value of Pn. 
                     * @example `01/11 05/11 Pn 05/09` or `9/11 Pn 05/09`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> CURSOR_LINE_TABULATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CVT), {1});
                }

                namespace displayControlFunctions {
                    
                    /**
                     * @brief NP causes the n-th following page in the presentation component to be displayed, where n equals the value of Pn.
                     * NOTE: The effect of this control function on the active presentation position is not defined by this Standard. 
                     * @example `01/11 05/11 Pn 05/05` or `9/11 Pn 05/05`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> NEXT_PAGE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::NP), {1});
                    
                    /**
                     * @brief PP causes the n-th preceding page in the presentation component to be displayed, where n equals the value of Pn. 
                     * The effect of this control function on the active presentation position is not defined by this Standard. 
                     * @example `01/11 05/11 Pn 05/06` or `9/11 Pn 05/06`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> PRECEDING_PAGE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::PP), {1});
                    
                    /**
                     * @brief SD causes the data in the presentation component to be moved by n line positions if the line orientation
                     * is horizontal, or by n character positions if the line orientation is vertical, such that the data appear to move down; 
                     * where n equals the value of Pn.
                     * NOTE: The active presentation position is not affected by this control function.
                     * @example `01/11 05/11 Pn 05/04` or `9/11 Pn 05/04`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> SCROLL_DOWN(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::SD), {1});
                    
                    /**
                     * @brief SL causes the data in the presentation component to be moved by n character positions if the line
                     * orientation is horizontal, or by n line positions if the line orientation is vertical, such that the data appear to move to the left; where n equals the value of Pn.
                     * NOTE: The active presentation position is not affected by this control function. 
                     * @example `01/11 05/11 Pn 02/00 04/00` or `9/11 Pn 02/00 04/00`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SCROLL_LEFT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SL), {1}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SR causes the data in the presentation component to be moved by n character positions if the line
                     * orientation is horizontal, or by n line positions if the line orientation is vertical, such that the data appear to move to the right; where n equals the value of Pn.
                     * NOTE: The active presentation position is not affected by this control function. 
                     * @example `01/11 05/11 Pn 02/00 04/01` or `9/11 Pn 02/00 04/01`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> SCROLL_RIGHT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SR), {1}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief SU causes the data in the presentation component to be moved by n line positions if the line orientation
                     * is horizontal, or by n character positions if the line orientation is vertical, such that the data appear to move up; where n equals the value of Pn.
                     * NOTE: The active presentation position is not affected by this control function. 
                     * @example `01/11 05/11 Pn 05/03` or `9/11 Pn 05/03`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> SCROLL_UP(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::SU), {1});
                }

                namespace deviceControlFunctions {
                    /**
                     * @brief DC1 is primarily intended for turning on or starting an ancillary device. 
                     * If it is not required for this purpose, it may be used to restore a device to the basic mode of operation (see also DC2 and DC3), or
                     * any other device control function not provided by other DCs.
                     * NOTE: When used for data flow control, DC1 is sometimes called "X-ON". 
                     * @example `01/01`
                     */
                     inline base<sequence::basic> DEVICE_CONTROL_ONE             = base<sequence::basic>(sequence::basic(table::C0::DC1));
                    
                    /**
                     * @brief DC2 is primarily intended for turning on or starting an ancillary device. 
                     * If it is not required for this purpose, it may be used to set a device to a special mode of operation (in which case DC1 is used to restore the device to the basic mode), 
                     * or for any other device control function not provided by other DCs.
                     * @example `01/02`
                     */
                    inline base<sequence::basic> DEVICE_CONTROL_TWO             = base<sequence::basic>(sequence::basic(table::C0::DC2));
                    
                    /**
                     * @brief DC3 is primarily intended for turning off or stopping an ancillary device. 
                     * This function may be a secondary level stop, for example wait, pause, stand-by or halt (in which case DC1 is used to restore normal operation). 
                     * If it is not required for this purpose, it may be used for any other device control function not provided by other DCs.
                     * NOTE: When used for data flow control, DC3 is sometimes called "X-OFF". 
                     * @example `01/03`
                     */
                    inline base<sequence::basic> DEVICE_CONTROL_THREE           = base<sequence::basic>(sequence::basic(table::C0::DC3));
                    
                    /**
                     * @brief DC4 is primarily intended for turning off, stopping or interrupting an ancillary device. 
                     * If it is not required for this purpose, it may be used for any other device control function not provided by other DCs. 
                     * @example `01/04`
                     */
                    inline base<sequence::basic> DEVICE_CONTROL_FOUR            = base<sequence::basic>(sequence::basic(table::C0::DC4));
                }

                namespace informationSeparators {
                    
                    /**
                     * @brief IS1 is used to separate and qualify data logically; its specific meaning has to be defined for each application. 
                     * If this control function is used in hierarchical order, it may delimit a data item called a unit, see 8.2.10. 
                     * @example `01/15`
                     */
                    inline base<sequence::basic> INFORMATION_SEPARATOR_ONE      = base<sequence::basic>(sequence::basic(table::C0::IS1));
                    
                    /**
                     * @brief IS2 is used to separate and qualify data logically; its specific meaning has to be defined for each application. 
                     * If this control function is used in hierarchical order, it may delimit a data item called a record, see 8.2.10. 
                     * @example `01/14`
                     */
                    inline base<sequence::basic> INFORMATION_SEPARATOR_TWO      = base<sequence::basic>(sequence::basic(table::C0::IS2));
                    
                    /**
                     * @brief IS3 is used to separate and qualify data logically; its specific meaning has to be defined for each application. 
                     * If this control function is used in hierarchical order, it may delimit a data item called a group, see 8.2.10. 
                     * @example `01/13`
                     */
                    inline base<sequence::basic> INFORMATION_SEPARATOR_THREE    = base<sequence::basic>(sequence::basic(table::C0::IS3));
                    
                    /**
                     * @brief IS4 is used to separate and qualify data logically; its specific meaning has to be defined for each application. 
                     * If this control function is used in hierarchical order, it may delimit a data item called a file, see 8.2.10. 
                     * @example `01/12`
                     */
                    inline base<sequence::basic> INFORMATION_SEPARATOR_FOUR     = base<sequence::basic>(sequence::basic(table::C0::IS4));
                }

                namespace areaDefinitions {
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> DEFINE_AREA_QUALIFICATION(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::DAQ), {});
                    
                    /**
                     * @brief EPA is used to indicate that the active presentation position is the last of a string of character positions
                     * in the presentation component, the contents of which are protected against manual alteration, are
                     * guarded against transmission or transfer, depending on the setting of the GUARDED AREA TRANSFER MODE (GATM), 
                     * and may be protected against erasure, depending on the setting of the ERASURE MODE (ERM). 
                     * The beginning of this string is indicated by START OF GUARDED AREA (SPA).
                     * NOTE: The control functions for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SRS string or an SDS string. 
                     * @example `09/07` or `01/11 05/07` 
                     */
                    inline base<sequence::basic> END_OF_GUARDED_AREA            = base<sequence::basic>(sequence::basic(table::C1::EPA));
                    
                    /**
                     * @brief ESA is used to indicate that the active presentation position is the last of a string of character positions
                     * in the presentation component, the contents of which are eligible to be transmitted in the form of a data
                     * stream or transferred to an auxiliary input/output device. 
                     * The beginning of this string is indicated by START OF SELECTED AREA (SSA).
                     * NOTE: The control function for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SRS string or an SDS string. 
                     * @example `08/07` or `01/11 04/07` 
                     */
                    inline base<sequence::basic> END_OF_SELECTED_AREA           = base<sequence::basic>(sequence::basic(table::C1::ESA));
                    
                    /**
                     * @brief SPA is used to indicate that the active presentation position is the first of a string of character positions
                     * in the presentation component, the contents of which are protected against manual alteration, are
                     * guarded against transmission or transfer, depending on the setting of the GUARDED AREA TRANSFER MODE (GATM) and may be protected against erasure, 
                     * depending on the setting of the ERASURE MODE (ERM). 
                     * The end of this string is indicated by END OF GUARDED AREA (EPA).
                     * NOTE: The control functions for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SRS string or an SDS string.
                     * @example `09/06` or `01/11 05/06` 
                     */
                    inline base<sequence::basic> START_OF_GUARDED_AREA          = base<sequence::basic>(sequence::basic(table::C1::SPA));
                    
                    /**
                     * @brief SSA is used to indicate that the active presentation position is the first of a string of character positions
                     * in the presentation component, the contents of which are eligible to be transmitted in the form of a data
                     * stream or transferred to an auxiliary input/output device.
                     * The end of this string is indicated by END OF SELECTED AREA (ESA). 
                     * The string of characters actually transmitted or transferred depends on the setting of the GUARDED AREA TRANSFER MODE (GATM) 
                     * and on any guarded areas established by DEFINE AREA QUALIFICATION (DAQ), 
                     * or by START OF GUARDED AREA (SPA) and END OF GUARDED AREA (EPA).
                     * NOTE: The control functions for area definition (DAQ, EPA, ESA, SPA, SSA) should not be used within an SRS string or an SDS string. 
                     * @example `08/06` or `01/11 04/06` 
                     */
                    inline base<sequence::basic> START_OF_SELECTED_AREA         = base<sequence::basic>(sequence::basic(table::C1::SSA));
                }

                namespace modeSettings {
                    /**
                     * @brief RM causes the modes of the receiving device to be reset as specified by the parameter values.
                     * NOTE: Private modes may be implemented using private parameters, see 5.4.1 and 7.4. 
                     * @example `01/11 05/11 Ps... 06/12` or `9/11 Ps... 06/12`
                     * @param Ps default(None)
                     * @param ...
                     */
                    namespace RESET_MODE {
                        enum class types {
                            GUARDED_AREA_TRANSFER_MODE          = 1,
                            KEYBOARD_ACTION_MODE,
                            CONTROL_REPRESENTATION_MODE,
                            INSERTION_REPLACEMENT_MODE,
                            STATUS_REPORT_TRANSFER_MODE,
                            ERASURE_MODE,
                            LINE_EDITING_MODE,
                            BI_DIRECTIONAL_SUPPORT_MODE,
                            DEVICE_COMPONENT_SELECT_MODE,
                            CHARACTER_EDITING_MODE,
                            POSITIONING_UNIT_MODE,
                            SEND_RECEIVE_MODE,
                            FORMAT_EFFECTOR_ACTION_MODE,
                            FORMAT_EFFECTOR_TRANSFER_MODE,
                            MULTIPLE_AREA_TRANSFER_MODE,
                            TRANSFER_TERMINATION_MODE,
                            SELECTED_AREA_TRANSFER_MODE,
                            TABULATION_STOP_MODE                = 18,
                            GRAPHIC_RENDITION_COMBINATION_MODE  = 21,
                            ZERO_DEFAULT_MODE,
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 0, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::RM), {});
                    }

                    /**
                     * @brief SM causes the modes of the receiving device to be set as specified by the parameter values.
                     * NOTE: Private modes may be implemented using private parameters, see 5.4.1 and 7.4. 
                     * @example `01/11 05/11 Ps... 06/08` or `9/11 Ps... 06/08`
                     * @param Ps default(None)
                     * @param ...
                     */
                    namespace SET_MODE {
                        using types = RESET_MODE::types;

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 0, specialTypes::HAS_INFINITE_PARAMETERS> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SM), {});
                    }
                }

                namespace transmissionControlFunctions {
                    
                    /**
                     * @brief ACK is transmitted by a receiver as an affirmative response to the sender.
                     * The use of ACK is defined in ISO 1745. 
                     * @example `00/06`
                     */
                    inline base<sequence::basic> ACKNOWLEDGE                    = base<sequence::basic>(sequence::basic(table::C0::ACK));
                    
                    /**
                     * @brief DLE is used exclusively to provide supplementary transmission control functions.
                     * The use of DLE is defined in ISO 1745. 
                     * @example `01/00`
                     */
                    inline base<sequence::basic> DATA_LINK_ESCAPE               = base<sequence::basic>(sequence::basic(table::C0::DLE));
                    
                    /**
                     * @brief ENQ is transmitted by a sender as a request for a response from a receiver.
                     * The use of ENQ is defined in ISO 1745. 
                     * @example `00/05`
                     */
                    inline base<sequence::basic> ENQUIRY                        = base<sequence::basic>(sequence::basic(table::C0::ENQ));
                    
                    /**
                     * @brief EOT is used to indicate the conclusion of the transmission of one or more texts.
                     * The use of EOT is defined in ISO 1745. 
                     * @example `00/04`
                     */
                    inline base<sequence::basic> END_OF_TRANSMISSION            = base<sequence::basic>(sequence::basic(table::C0::EOT));
                    
                    /**
                     * @brief ETB is used to indicate the end of a block of data where the data are divided into such blocks for transmission purposes.
                     * The use of ETB is defined in ISO 1745. 
                     * @example `01/07`
                     */
                    inline base<sequence::basic> END_OF_TRANSMISSION_BLOCK      = base<sequence::basic>(sequence::basic(table::C0::ETB));
                    
                    /**
                     * @brief ETX is used to indicate the end of a text.
                     * The use of ETX is defined in ISO 1745.
                     * @example `00/03`
                     */
                    inline base<sequence::basic> END_OF_TEXT                    = base<sequence::basic>(sequence::basic(table::C0::ETX));
                    
                    /**
                     * @brief NAK is transmitted by a receiver as a negative response to the sender.
                     * The use of NAK is defined in ISO 1745.
                     * @example `01/05`
                     */
                    inline base<sequence::basic> NEGATIVE_ACKNOWLEDGE           = base<sequence::basic>(sequence::basic(table::C0::NAK));
                    
                    /**
                     * @brief SOH is used to indicate the beginning of a heading.
                     * The use of SOH is defined in ISO 1745. 
                     * @example `00/01`
                     */
                    inline base<sequence::basic> START_OF_HEADING               = base<sequence::basic>(sequence::basic(table::C0::SOH));
                    
                    /**
                     * @brief STX is used to indicate the beginning of a text and the end of a heading.
                     * The use of STX is defined in ISO 1745. 
                     * @example `00/02`
                     */
                    inline base<sequence::basic> START_OF_TEXT                  = base<sequence::basic>(sequence::basic(table::C0::STX));
                    
                    /**
                     * @brief SYN is used by a synchronous transmission system in the absence of any other character (idle condition) to
                     * provide a signal from which synchronism may be achieved or retained between data terminal equipment.
                     * The use of SYN is defined in ISO 1745. 
                     * @example `01/06`
                     */
                    inline base<sequence::basic> SYNCHRONOUS_IDLE               = base<sequence::basic>(sequence::basic(table::C0::SYN));
                }

                namespace miscellaneousControlFunctions {
                    
                    /**
                     * @brief BEL is used when there is a need to call for attention; it may control alarm or attention devices.
                     * @example `00/07`
                     */
                    inline base<sequence::basic> BELL                           = base<sequence::basic>(sequence::basic(table::C0::BEL));
                    
                    /**
                     * @brief CAN is used to indicate that the data preceding it in the data stream is in error. 
                     * As a result, this data shall be ignored. 
                     * The specific meaning of this control function shall be defined for each application and/or between sender and recipient.
                     * @example `01/08`
                     */
                    inline base<sequence::basic> CANCEL                         = base<sequence::basic>(sequence::basic(table::C0::CAN));
                    
                    /**
                     * @brief CCH is used to indicate that both the preceding graphic character in the data stream, 
                     * (represented by one or more bit combinations) including SPACE, and the control function CCH itself are to be ignored for further interpretation of the data stream.
                     * If the character preceding CCH in the data stream is a control function (represented by one or more bit combinations), the effect of CCH is not defined by this Standard
                     * @example `09/04` or `01/11 05/04` 
                     */
                    inline base<sequence::basic> CANCEL_CHARACTER               = base<sequence::basic>(sequence::basic(table::C1::CCH));
                    
                    /**
                     * @brief This sequence means two different things based on previous sequence.
                     * 1) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, CPR is used to
                     * report the active presentation position of the sending device as residing in the presentation component at
                     * the n-th line position according to the line progression and at the m-th character position according to
                     * the character path, where n equals the value of Pn1 and m equals the value of Pn2.
                     * 2) If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, CPR is used to report the
                     * active data position of the sending device as residing in the data component at the n-th line position
                     * according to the line progression and at the m-th character position according to the character
                     * progression, where n equals the value of Pn1 and m equals the value of Pn2.
                     * NOTE: CPR may be solicited by a DEVICE STATUS REPORT (DSR) or be sent unsolicited. 
                     * @example `01/11 05/11 Pn1;Pn2 05/02` or `9/11 Pn1;Pn2 05/02`
                     * @param Pn1 default(1)
                     * @param Pn2 default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2> ACTIVE_POSITION_REPORT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::CPR), {1, 1});
                    
                    /**
                     * @brief With a parameter value not equal to 0, DA is used to identify the device which sends the DA. 
                     * The parameter value is a device type identification code according to a register which is to be established. 
                     * If the parameter value is 0, DA is used to request an identifying DA from a device.
                     * @example `01/11 05/11 Ps 06/03` or `9/11 Ps 06/03`
                     * @param Ps default(0)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> DEVICE_ATTRIBUTES(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::DA), {0});
                    
                    /**
                     * @brief DMI causes the manual input facilities of a device to be disabled.
                     * @example `01/11 06/00` 
                     */
                    inline base<sequence::independent> DISABLE_MANUAL_INPUT = base<sequence::independent>(sequence::independent(table::independentFunctions::DMI));
                    
                    /**
                     * @brief DSR is used either to report the status of the sending device or to request a status report from the receiving device, depending on the parameter values.
                     * NOTE: DSR with parameter value 0, 1, 2, 3 or 4 may be sent either unsolicited or as a response to a request such as a DSR with a parameter value 5 or MESSAGE WAITING (MW). 
                     * @example `01/11 05/11 Ps 06/14` or `9/11 Ps 06/14`
                     * @param Ps default(0)
                     */
                    namespace DEVICE_STATUS_REPORT {
                        enum class types {
                            READY,                                  // Ready, no malfunction detected
                            BUSY_REQUEST_LATER,                     // Busy, another DSR must be requested later
                            BUSY_SEND_LATER,                        // Busy, another DSR will be sent later 
                            ERROR_REQUEST_LATER,                    // Some malfunction detected, another DSR must be requested later 
                            ERROR_SEND_LATER,                       // Some malfunction detected, another DSR will be sent later 
                            DSR_REQUESTED,                          // A DSR is requested
                            ACTIVE_POSITION_REQUESTED,              // A report of the active presentation position or of the active data position in the form of ACTIVE POSITION REPORT (CPR) is requested 
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::DSR), {types::READY});
                    }

                    /**
                     * @brief EM is used to identify the physical end of a medium, or the end of the used portion of a medium, or the end of the wanted portion of data recorded on a medium.
                     * @example `01/09`
                     */
                    inline base<sequence::basic> END_OF_MEDIUM                  = base<sequence::basic>(sequence::basic(table::C0::EM));
                    
                    /**
                     * @brief EMI is used to enable the manual input facilities of a device.
                     * @example `01/11 06/02`
                     */
                    inline base<sequence::independent> ENABLE_MANUAL_INPUT = base<sequence::independent>(sequence::independent(table::independentFunctions::EMI));
                    
                    /**
                     * @brief FNK is a control function in which the parameter value identifies the function key which has been operated. 
                     * @example `01/11 05/11 Pn 02/00 05/07` or `9/11 Pn 02/00 05/07`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> FUNCTION_KEY(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::FNK), {}, {table::toInt(2, 00)});
                    
                    /**
                     * @brief IDCS is used to specify the purpose and format of the command string of subsequent DEVICE CONTROL STRINGs (DCS). 
                     * The specified purpose and format remain in effect until the next occurrence of IDCS in the data stream. 
                     * The format and interpretation of the command string corresponding to these parameter values are to be defined in appropriate standards. 
                     * if this control function is used to identify a private command string, a private parameter value shall be used
                     * @example `01/11 05/11 Ps 02/00 04/15` or `9/11 Ps 02/00 04/15`
                     * @param Ps default(None)
                     */
                    namespace IDENTIFY_DEVICE_CONTROL_STRING {
                        enum class types {
                            DIAGNOSTIC_STATE_OF_STATUS_REPORT_TRANSFER_MODE,            // Reserved for use with the DIAGNOSTIC state of the STATUS REPORT TRANSFER MODE (SRTM)
                            DYNAMICALLY_REDEFINE_CHARACTER_SETS                         // Reserved for Dynamically Redefinable Character Sets (DRCS) according to Standard ECMA-35. 
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithIntermediate::IDCS), {}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief IGS is used to indicate that a repertoire of the graphic characters of ISO/IEC 10367 is used in the subsequent text.
                     * The parameter value of IGS identifies a graphic character repertoire registered in accordance with ISO/IEC 7350. 
                     * @example `01/11 05/11 Ps 02/00 04/13` or `9/11 Ps 02/00 04/13`
                     * @param Ps default(None)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1, 1> IDENTIFY_GRAPHIC_SUBREPERTOIRE(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::IGS), {}, {table::toInt(2, 00)});

                    /**
                     * @brief INT is used to indicate to the receiving device that the current process is to be interrupted and an agreed procedure is to be initiated. 
                     * This control function is applicable to either direction of transmission. 
                     * @example `01/11 06/01`
                     */
                    inline base<sequence::independent> INTERRUPT = base<sequence::independent>(sequence::independent(table::independentFunctions::INT));

                    /**
                     * @brief MC is used either to initiate a transfer of data from or to an auxiliary input/output device or to enable or disable the relay of the received data stream to an auxiliary input/output device, depending on the parameter value.
                     * NOTE: This control function may not be used to switch on or off an auxiliary device. 
                     * @example `01/11 05/11 Ps 06/09` or `9/11 Ps 06/09`
                     * @param Ps default(0)
                     */
                    namespace MEDIA_COPY {
                        enum class types {
                            TRANSFER_TO_PRIMARY_AUXILIARY_DEVICE,           //initiate transfer to a primary auxiliary device
                            TRANSFER_FROM_PRIMARY_AUXILIARY_DEVICE,         //initiate transfer from a primary auxiliary device
                            TRANSFER_TO_SECONDARY_AUXILIARY_DEVICE,         //initiate transfer to a secondary auxiliary device
                            TRANSFER_FROM_SECONDARY_AUXILIARY_DEVICE,       //initiate transfer from a secondary auxiliary device
                            STOP_RELAY_TOPRIMARY_AUXILIARY_DEVICE,          //stop relay to a primary auxiliary device
                            START_RELAY_TO_PRIMARY_AUXILIARY_DEVICE,        //start relay to a primary auxiliary device
                            STOP_RELAY_TO_SECONDARY_AUXILIARY_DEVICE,       //stop relay to a secondary auxiliary device
                            START_RELAY_TO_SECONDARY_AUXILIARY_DEVICE,      //start relay to a secondary auxiliary device 
                        };  

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::MC), {types::TRANSFER_TO_PRIMARY_AUXILIARY_DEVICE});
                    }

                    /**
                     * @brief MW is used to set a message waiting indicator in the receiving device. 
                     * An appropriate acknowledgement to the receipt of MW may be given by using DEVICE STATUS REPORT (DSR).
                     * @example `09/05` or `01/11 05/05` 
                     */
                    inline base<sequence::basic> MESSAGE_WAITING                = base<sequence::basic>(sequence::basic(table::C1::MW));

                    /**
                     * @brief NUL is used for media-fill or time-fill. NUL characters may be inserted into, or removed from, a data
                     * stream without affecting the information content of that stream, but such action may affect the information layout and/or the control of equipment. 
                     * @example `00/00`
                     */
                    inline base<sequence::basic> NULL_CHARACTER                 = base<sequence::basic>(sequence::basic(table::C0::NUL));

                    /**
                     * @brief PU1 is reserved for a function without standardized meaning for private use as required, subject to the prior agreement between the sender and the recipient of the data. 
                     * @example `09/01` or `01/11 05/01`
                     */
                    inline base<sequence::basic> PRIVATE_USE_ONE                = base<sequence::basic>(sequence::basic(table::C1::PU1));

                    /**
                     * @brief PU2 is reserved for a function without standardized meaning for private use as required, subject to the prior agreement between the sender and the recipient of the data
                     * @example `09/02` or `01/11 05/02`
                     */
                    inline base<sequence::basic> PRIVATE_USE_TWO                = base<sequence::basic>(sequence::basic(table::C1::PU2));

                    /**
                     * @brief REP is used to indicate that the preceding character in the data stream, if it is a graphic character
                     * (represented by one or more bit combinations) including SPACE, is to be repeated n times, where n
                     * equals the value of Pn. If the character preceding REP is a control function or part of a control function,
                     * the effect of REP is not defined by this Standard. 
                     * @example `01/11 05/11 Pn 06/02` or `9/11 Pn 06/02`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 1> REPEAT(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithoutIntermediate::REP), {1});

                    /**
                     * @brief RIS causes a device to be reset to its initial state, i.e. the state it has after it is made operational. 
                     * This may imply, if applicable: clear tabulation stops, remove qualified areas, reset graphic rendition, put all
                     * character positions into the erased state, move the active presentation position to the first position of the
                     * first line in the presentation component, move the active data position to the first character position of
                     * the first line in the data component, set the modes into the reset state, etc. 
                     * @example `01/11 06/03`
                     */
                    inline base<sequence::independent> RESET_TO_INITIAL_STATE = base<sequence::independent>(sequence::independent(table::independentFunctions::RIS));

                    /**
                     * @brief SEE is used to establish the editing extent for subsequent character or line insertion or deletion. 
                     * The established extent remains in effect until the next occurrence of SEE in the data stream. 
                     * The editing extent depends on the parameter value.
                     * @example `01/11 05/11 Ps 05/01` or `9/11 Ps 05/01`
                     * @param Ps default(0)
                     */
                    namespace SELECT_EDITING_EXTENT {
                        enum class types {
                            ACTIVE_PAGE,                    // the shifted part is limited to the active page in the presentation component
                            ACTIVE_LINE,                    // the shifted part is limited to the active line in the presentation component
                            ACTIVE_FIELD,                   // the shifted part is limited to the active field in the presentation component
                            ACTIVE_QUALIFIED_AREA,          // the shifted part is limited to the active qualified area
                            ENTIRE_PRESENTATION_COMPONENT   // the shifted part consists of the relevant part of the entire presentation component
                        };

                        inline base<sequence::controlSequence<sequence::parameter::selectable<types>>, types, 1> code(sequence::controlSequence<sequence::parameter::selectable<types>>(table::finalWithoutIntermediate::SSE), {types::ACTIVE_PAGE});
                    }

                    /**
                     * @brief SEF causes a sheet of paper to be ejected from a printing device into a specified output stacker and
                     * another sheet to be loaded into the printing device from a specified paper bin.
                     * NOTE: Normally the parameters are Ps1;Ps2, but since they have an unlimited amount of value and only the zero value is special, GGUI will treat them as Pn1;Pn2
                     * @example `01/11 05/11 Pn1;Pn2 02/00 05/09` or `9/11 Pn1;Pn2 02/00 05/09`
                     * @param Pn1 default(0) - 0: eject sheet, no new sheet loaded; 1-n: eject sheet and load another from bin n
                     * @param Pn2 default(0) - 0: eject sheet, no stacker specified; 1-n: eject sheet into stacker n
                     */
                    namespace SHEET_EJECT_AND_FEED {
                        inline base<sequence::controlSequence<sequence::parameter::numeric>, sequence::parameter::numeric, 2, 1> code(sequence::controlSequence<sequence::parameter::numeric>(table::finalWithIntermediate::SEF), {0, 0}, {table::toInt(2, 00)});
                    }

                    /**
                     * @brief STS is used to establish the transmit state in the receiving device. 
                     * In this state the transmission of data from the device is possible.
                     * The actual initiation of transmission of data is performed by a data communication or input/output
                     * interface control procedure which is outside the scope of this Standard.
                     * The transmit state is established either by STS appearing in the received data stream or by the operation of an appropriate key on a keyboard. 
                     * @example `09/03` or `01/11 05/03`
                     */
                    inline base<sequence::basic> SET_TRANSMIT_STATE             = base<sequence::basic>(sequence::basic(table::C1::STS));

                    /**
                     * @brief SUB is used in the place of a character that has been found to be invalid or in error. 
                     * SUB is intended to be introduced by automatic means.
                     * @example `01/10`
                     */
                    inline base<sequence::basic> SUBSTITUTE                     = base<sequence::basic>(sequence::basic(table::C0::SUB));
                }
            }

        }
    }
}

#endif