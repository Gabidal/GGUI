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

                class parameter {
                protected:
                    std::vector<uint32_t> subNumbers;       // For instances where 1:2, these can be used as decimals.
                public:
                    constexpr static uint8_t sub_delimeter  = table::toInt(3, 10); // Translates into ':'
                    constexpr static uint8_t delimeter      = table::toInt(3, 11); // Translates into ';'

                    parameter(std::string_view input, size_t& length);
                    parameter(std::vector<uint32_t> values) : subNumbers(values) {}
                    parameter(uint32_t values) : subNumbers({values}) {}

                    std::string toString();
                };

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

                class controlSequence : public base {
                protected:
                    std::vector<parameter> parameters;          // Each range between: 03/00 - 03/15, delimeetered by 03/11 (';')
                    std::vector<uint8_t> intermediates;         // Each range between: 02/00 - 02/15
                    std::variant<
                        table::finalWithoutIntermediate,        // Each range between: 04/00 - 07/14
                        table::finalWithIntermediate            // Each range between: 04/00 - 07/14
                    > finalByte;
                public:
                    static void parse(std::string_view input, size_t& length, std::vector<base*>& output);

                    controlSequence(
                        std::vector<parameter> params,
                        table::finalWithoutIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameters(params), finalByte(finalByte) {}

                    controlSequence(
                        std::vector<parameter> params,
                        std::vector<uint8_t> inters,
                        table::finalWithIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameters(params), intermediates(inters), finalByte(finalByte) {}

                    controlSequence(
                        table::finalWithoutIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), finalByte(finalByte) {}

                    controlSequence(
                        table::finalWithIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), intermediates({table::toInt(2, 0)}), finalByte(finalByte) {}

                    std::string toString() override;
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
                    ) : base(specialType::INDEPENDENT, bitType::_7bit), function(func), hasSpace(space) {}

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
                template<
                    typename codeType,
                    typename parameterType              = sequence::parameter,
                    std::size_t paramCount              = 0,
                    std::size_t intermediateCount       = 0
                >
                class base {
                public:
                    codeType function;
                    std::array<parameterType, paramCount> parameterDefaultValue;
                    std::array<uint8_t, intermediateCount> intermediateDefaultValues;

                    base(codeType code, std::array<parameterType, paramCount> defaultParamValues = {}, std::array<uint8_t, intermediateCount> defaultIntermediates = {}) : function(code), parameterDefaultValue(defaultValue), intermediateDefaultValues(defaultIntermediates) {}
                };

                namespace delimiters {
                    /**
                     * @brief APC is used as the opening delimiter of a control string for application program use. The command
                        string following may consist of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14. The
                        control string is closed by the terminating delimiter STRING TERMINATOR (ST). The interpretation of
                        the command string depends on the relevant application program. 
                     * @example `09/15` or `01/11 05/15`
                    */
                    inline base<sequence::basic> APPLICATION_PROGRAM_COMMAND(sequence::basic(table::C1::APC));

                    /**
                     * @brief CMD is used as the delimiter of a string of data coded according to Standard ECMA-35 and to switch to
                        a general level of control. The use of CMD is not mandatory if the higher level protocol defines means of delimiting the string, 
                        for instance, by specifying the length of the string. 
                     * @example `01/11 06/04`
                     */
                    inline base<sequence::independent> CODING_METHOD_DELIMITER(sequence::independent(table::independentFunctions::CMD));

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
                    inline base<sequence::basic> DEVICE_CONTROL_STRING(sequence::basic(table::C1::DCS));

                    /**
                     * @brief OSC is used as the opening delimiter of a control string for operating system use. The command string
                        following may consist of a sequence of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14.
                        The control string is closed by the terminating delimiter STRING TERMINATOR (ST). The
                        interpretation of the command string depends on the relevant operating system. 
                     * @example `09/13` or `01/11 05/13` 
                     */
                    inline base<sequence::basic> OPERATING_SYSTEM_COMMAND(sequence::basic(table::C1::OSC));

                    /**
                     * @brief PM is used as the opening delimiter of a control string for privacy message use. The command string
                        following may consist of a sequence of bit combinations in the range 00/08 to 00/13 and 02/00 to 07/14.
                        The control string is closed by the terminating delimiter STRING TERMINATOR (ST). The
                        interpretation of the command string depends on the relevant privacy discipline.
                     * @example `09/14` or `01/11 05/14` 
                     */
                    inline base<sequence::basic> PRIVACY_MESSAGE(sequence::basic(table::C1::PM));

                    /**
                     * @brief SOS is used as the opening delimiter of a control string. The character string following may consist of
                        any bit combination, except those representing SOS or STRING TERMINATOR (ST). The control string
                        is closed by the terminating delimiter STRING TERMINATOR (ST). The interpretation of the character
                        string depends on the application.
                     * @example `09/08` or `01/11 05/08`
                     */
                    inline base<sequence::basic> START_OF_STRING(sequence::basic(table::C1::SOS));

                    /**
                     * @brief ST is used as the closing delimiter of a control string opened by APPLICATION PROGRAM
                        COMMAND (APC), DEVICE CONTROL STRING (DCS), OPERATING SYSTEM COMMAND
                        (OSC), PRIVACY MESSAGE (PM), or START OF STRING (SOS).
                     * @example `09/12` or `01/11 05/12` 
                     */
                    inline base<sequence::basic> STRING_TERMINATOR(sequence::basic(table::C1::ST));
                };

                namespace introducers {
                    /**
                     * @brief CSI is used as the first character of a control sequence.
                     * @example `09/11` or `01/11 05/11`
                     */
                    inline base<sequence::basic> CONTROL_SEQUENCE_INTRODUCER(sequence::basic(table::C1::CSI));

                    /**
                     * @brief ESC is used for code extension purposes. It causes the meanings of a limited number of bit combinations
                        following it in the data stream to be changed. 
                     * @example `01/11`
                     */
                    inline base<sequence::basic> ESCAPE(sequence::basic(table::C0::ESC));

                    /**
                     * @brief SCI and the bit combination following it are used to represent a control function or a graphic character.
                        The bit combination following SCI must be from 00/08 to 00/13 or 02/00 to 07/14. The use of SCI is
                        reserved for future standardization.
                     * @example `09/10` or `01/11 05/10`
                     */
                    inline base<sequence::basic> SINGLE_CHARACTER_INTRODUCER(sequence::basic(table::C1::SCI));
                }

                namespace shiftFunctions {
                    /**
                     * @brief LS0 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                     * @example `00/15`
                     */
                    inline base<sequence::basic> LOCKING_SHIFT_ZERO(sequence::basic(table::C0::LS0));

                    /**
                     * @brief LS1 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                     * @example `00/14`
                     */
                    inline base<sequence::basic> LOCKING_SHIFT_ONE(sequence::basic(table::C0::LS1));

                    /**
                     * @brief LS1R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `07/14`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_ONE_RIGHT(sequence::independent(table::independentFunctions::LS1R));

                    /**
                     * @brief LS2 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 06/14`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_TWO(sequence::independent(table::independentFunctions::LS2));

                    /**
                     * @brief LS2R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed.
                    * @example `01/11 07/13`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_TWO_RIGHT(sequence::independent(table::independentFunctions::LS2R));

                    /**
                     * @brief LS3 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 06/15`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_THREE(sequence::independent(table::independentFunctions::LS3));

                    /**
                     * @brief LS3R is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `01/11 07/12`
                     */
                    inline base<sequence::independent> LOCKING_SHIFT_THREE_RIGHT(sequence::independent(table::independentFunctions::LS3R));

                    /**
                     * @brief SI is used for code extension purposes. It causes the meanings of the bit combinations following it in the
                        data stream to be changed. 
                     * @example `00/15`
                     */
                    inline base<sequence::basic> SHIFT_IN(sequence::basic(table::C0::SI));

                    /**
                     * @brief SO is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `00/14`
                     */
                    inline base<sequence::basic> SHIFT_OUT(sequence::basic(table::C0::SO));

                    /**
                     * @brief SS2 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `08/14` or `01/11 04/14`
                     */
                    inline base<sequence::basic> SS2(sequence::basic(table::C1::SS2));

                    /**
                     * @brief SS3 is used for code extension purposes. It causes the meanings of the bit combinations following it in
                        the data stream to be changed. 
                     * @example `08/15` or `01/11 04/15` 
                     */
                    inline base<sequence::basic> SS3(sequence::basic(table::C1::SS3));

                }

                namespace formatEffectors {
                    /**
                     * @brief BS causes the active data position to be moved one character position in the data component in the
                        direction opposite to that of the implicit movement.
                        The direction of the implicit movement depends on the parameter value of SELECT IMPLICIT
                        MOVEMENT DIRECTION (SIMD). 
                     * @example `00/08`
                     */
                    inline base<sequence::basic> BACKSPACE(sequence::basic(table::C0::BS));

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
                    inline base<sequence::basic> CARRIAGE_RETURN(sequence::basic(table::C0::CR));

                    /**
                     * @brief FF causes the active presentation position to be moved to the corresponding character position of the
                        line at the page home position of the next form or page in the presentation component. The page home
                        position is established by the parameter value of SET PAGE HOME (SPH). 
                     * @example `00/12`
                     */
                    inline base<sequence::basic> FORM_FEED(sequence::basic(table::C0::FF));

                    /**
                     * @brief HPA causes the active data position to be moved to character position n in the active line (the line in the
                        data component that contains the active data position), where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/00` or `9/11 Pn 06/00`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> CHARACTER_POSITION_ABSOLUTE(sequence::controlSequence(table::finalWithoutIntermediate::HPA), {1});

                    /**
                     * @brief HPB causes the active data position to be moved by n character positions in the data component in the
                        direction opposite to that of the character progression, where n equals the value of Pn.
                     * @example `01/11 05/11 Pn 06/10` or `9/11 Pn 06/10`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> CHARACTER_POSITION_BACKWARD(sequence::controlSequence(table::finalWithoutIntermediate::HPB), {1});

                    /**
                     * @brief HPR causes the active data position to be moved by n character positions in the data component in the
                        direction of the character progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/01` or `9/11 Pn 06/01`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> CHARACTER_POSITION_FORWARD(sequence::controlSequence(table::finalWithoutIntermediate::HPR), {1});

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
                    inline base<sequence::basic> CHARACTER_TABULATION(sequence::basic(table::C0::HT));

                    /**
                     * @brief HTJ causes the contents of the active field (the field in the presentation component that contains the
                        active presentation position) to be shifted forward so that it ends at the character position preceding the
                        following character tabulation stop. The active presentation position is moved to that following character
                        tabulation stop. The character positions which precede the beginning of the shifted string are put into the
                        erased state. 
                     * @example `08/09` or `01/11 04/09` 
                     */
                    inline base<sequence::basic> CHARACTER_TABULATION_WITH_JUSTIFICATION(sequence::basic(table::C1::HTJ));

                    /**
                     * @brief HTS causes a character tabulation stop to be set at the active presentation position in the presentation
                        component.
                        The number of lines affected depends on the setting of the TABULATION STOP MODE (TSM). 
                     * @example `08/08` or `01/11 04/08`
                     */
                    inline base<sequence::basic> CHARACTER_TABULATION_SET(sequence::basic(table::C1::HTS));

                    /**
                     * @brief HVP causes the active data position to be moved in the data component to the n-th line position
                        according to the line progression and to the m-th character position according to the character
                        progression, where n equals the value of Pn1 and m equals the value of Pn2. 
                     * @example `01/11 05/11 Pn1;Pn2 06/06` or `9/11 Pn1;Pn2 06/06`
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 2> CHARACTER_AND_LINE_POSITION(sequence::controlSequence(table::finalWithoutIntermediate::HVP), {1, 1});

                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, LF causes the
                        active presentation position to be moved to the corresponding character position of the following line in
                        the presentation component.
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, LF causes the active data
                        position to be moved to the corresponding character position of the following line in the data
                        component. 
                     * @example `00/10`
                     */
                    inline base<sequence::basic> LINE_FEED(sequence::basic(table::C0::LF));

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
                    inline base<sequence::basic> NEXT_LINE(sequence::basic(table::C1::NEL));

                    /**
                     * @brief PLD causes the active presentation position to be moved in the presentation component to the
                        corresponding position of an imaginary line with a partial offset in the direction of the line progression.
                        This offset should be sufficient either to image following characters as subscripts until the first
                        following occurrence of PARTIAL LINE BACKWARD (PLU) in the data stream, or, if preceding
                        characters were imaged as superscripts, to restore imaging of following characters to the active line (the
                        line that contains the active presentation position).
                     * @example `08/11` or `01/11 04/11`
                     */
                    inline base<sequence::basic> PARTIAL_LINE_FORWARD(sequence::basic(table::C1::PLD));

                    /**
                     * @brief PLU causes the active presentation position to be moved in the presentation component to the
                        corresponding position of an imaginary line with a partial offset in the direction opposite to that of the
                        line progression. This offset should be sufficient either to image following characters as superscripts
                        until the first following occurrence of PARTIAL LINE FORWARD (PLD) in the data stream, or, if
                        preceding characters were imaged as subscripts, to restore imaging of following characters to the active
                        line (the line that contains the active presentation position). 
                     * @example `08/12` or `01/11 04/12` 
                     */
                    inline base<sequence::basic> PARTIAL_LINE_BACKWARD(sequence::basic(table::C1::PLU));

                    /**
                     * @brief PPA causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/00` or `9/11 Pn 02/00 05/00`
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1, 1> PAGE_POSITION_ABSOLUTE(sequence::controlSequence(table::finalWithIntermediate::PPA), {1}, {table::toInt(02, 00)});

                    /**
                     * @brief PPB causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th preceding page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/02` or `9/11 Pn 02/00 05/02`
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1, 1> PAGE_POSITION_BACKWARD(sequence::controlSequence(table::finalWithIntermediate::PPB), {1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief PPR causes the active data position to be moved in the data component to the corresponding character
                        position on the n-th following page, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 05/01` or `9/11 Pn 02/00 05/01`
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1, 1> PAGE_POSITION_FORWARD(sequence::controlSequence(table::finalWithIntermediate::PPR), {1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief If the DEVICE COMPONENT SELECT MODE (DCSM) is set to PRESENTATION, RI causes the
                        active presentation position to be moved in the presentation component to the corresponding character
                        position of the preceding line.
                        If the DEVICE COMPONENT SELECT MODE (DCSM) is set to DATA, RI causes the active data
                        position to be moved in the data component to the corresponding character position of the preceding line.
                     @example `08/13` or `ESC 04/13`
                     */
                    inline base<sequence::basic> REVERSE_LINE_FEED(sequence::basic(table::C1::RI));

                    /**
                     * @brief TBC causes one or more tabulation stops in the presentation component to be cleared, depending on the
                        parameter value. In the case of parameter value 0 or 2 the number of lines affected depends on the setting of the
                        TABULATION STOP MODE (TSM).
                     * @example `01/11 05/11 Ps 06/07` or `9/11 Ps 06/07`
                     */
                    namespace TABULATION_CLEAR {
                        enum class types {
                            ACTIVE_CHARACTER_POSITION,
                            ACTIVE_LINE_TABULATION,
                            ALL_CHARACTERS_IN_ACTIVE_LINE,
                            ALL_SINGLE_TABULATOR_CHARACTERS,
                            ALL_LINE_TABULATOR_CHARACTERS,
                            ALL_LINE_AND_SINGLE_TABULATOR_CHARACTERS
                        };

                        inline base<sequence::controlSequence, types, 1> code(sequence::controlSequence(table::finalWithoutIntermediate::TBC), {types::ACTIVE_CHARACTER_POSITION});
                    }

                    /**
                     * @brief TSR causes any character tabulation stop at character position n in the active line (the line that contains
                        the active presentation position) and lines of subsequent text in the presentation component to be
                        cleared, but does not affect other tabulation stops. n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 02/00 06/04` or `9/11 Pn 02/00 06/04`
                     * @param Pn default(None)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1, 1> TABULATION_STOP_REMOVE(sequence::controlSequence(table::finalWithIntermediate::TSR), {-1}, {table::toInt(02, 00)});
                    
                    /**
                     * @brief VPA causes the active data position to be moved to line position n in the data component in a direction
                        parallel to the line progression, where n equals the value of Pn.
                     * @example `01/11 05/11 Pn 06/04` or `9/11 Pn 06/04`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> LINE_POSITION_ABSOLUTE(sequence::controlSequence(table::finalWithoutIntermediate::VPA), {1});

                    /**
                     * @brief VPB causes the active data position to be moved by n line positions in the data component in a direction
                        opposite to that of the line progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/11` or `9/11 Pn 06/11`
                     * @param Pn default(1)
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> LINE_POSITION_BACKWARD(sequence::controlSequence(table::finalWithoutIntermediate::VPB), {1});

                    /**
                     * @brief VPR causes the active data position to be moved by n line positions in the data component in a direction
                        parallel to the line progression, where n equals the value of Pn. 
                     * @example `01/11 05/11 Pn 06/05` or `9/11 Pn 06/05`
                     */
                    inline base<sequence::controlSequence, sequence::parameter, 1> LINE_POSITION_FORWARD(sequence::controlSequence(table::finalWithoutIntermediate::VPR), {1});

                    /**
                     * @brief VT causes the active presentation position to be moved in the presentation component to the
                        corresponding character position on the line at which the following line tabulation stop is set. 
                     * @example `00/11`
                     */
                    inline base<sequence::basic> LINE_TABULATION(sequence::basic(table::C0::VT));

                    /**
                     * @brief VTS causes a line tabulation stop to be set at the active line (the line that contains the active presentation position). 
                     * @example `08/10` or `01/11 04/10`
                     */
                    inline base<sequence::basic> LINE_TABULATION_SET(sequence::basic(table::C1::VTS));
                }

                namespace presentationControlFunctions {
                    inline const auto BREAK_PERMITTED_HERE                          =       sequence::basic(table::C1::BPH);
                    inline const auto DIMENSION_TEXT_AREA                           =       sequence::controlSequence(2, table::finalWithIntermediate::DTA);
                    inline const auto FONT_SELECTION                                =       sequence::controlSequence(2, table::finalWithIntermediate::FNT);
                    inline const auto GRAPHIC_CHARACTER_COMBINATION                 =       sequence::controlSequence(1, table::finalWithIntermediate::GCC);
                    inline const auto GRAPHIC_SIZE_MODIFICATION                     =       sequence::controlSequence(2, table::finalWithIntermediate::GSM);
                    inline const auto GRAPHIC_SIZE_SELECTION                        =       sequence::controlSequence(1, table::finalWithIntermediate::GSS);
                    inline const auto JUSTIFY                                       =       sequence::controlSequence(-1, table::finalWithIntermediate::JFY);
                    inline const auto NO_BREAK_HERE                                 =       sequence::basic(table::C1::NBH);
                    inline const auto PRESENTATION_EXPAND_OR_CONTRACT               =       sequence::controlSequence(1, table::finalWithIntermediate::PEC);
                    inline const auto PAGE_FORMAT_SELECTION                         =       sequence::controlSequence(1, table::finalWithIntermediate::PFS);
                    inline const auto PARALLEL_TEXTS                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::PTX);
                    inline const auto QUAD                                          =       sequence::controlSequence(-1, table::finalWithIntermediate::QUAD);
                    inline const auto SET_ADDITIONAL_CHARACTER_SEPARATION           =       sequence::controlSequence(1, table::finalWithIntermediate::SACS);
                    inline const auto SELECT_ALTERNATIVE_PRESENTATION_VARIANTS      =       sequence::controlSequence(-1, table::finalWithIntermediate::SAPV);
                    inline const auto SET_CHARACTER_ORIENTATION                     =       sequence::controlSequence(1, table::finalWithIntermediate::SCO);
                    inline const auto SELECT_CHARACTER_PATH                         =       sequence::controlSequence(2, table::finalWithIntermediate::SCP);
                    inline const auto SET_CHARACTER_SPACING                         =       sequence::controlSequence(1, table::finalWithIntermediate::SCS);
                    inline const auto START_DIRECTED_STRING                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::SDS);
                    inline const auto SELECT_GRAPHIC_RENDITION                      =       sequence::controlSequence(-1, table::finalWithoutIntermediate::SGR);
                    inline const auto SELECT_CHARACTER_SPACING                      =       sequence::controlSequence(1, table::finalWithIntermediate::SHS);
                    inline const auto SELECT_IMPLICIT_MOVEMENT_DIRECTION            =       sequence::controlSequence(1, table::finalWithoutIntermediate::SIMD);
                    inline const auto SET_LINE_HOME                                 =       sequence::controlSequence(1, table::finalWithIntermediate::SHL);
                    inline const auto SET_LINE_LIMIT                                =       sequence::controlSequence(1, table::finalWithIntermediate::SLL);
                    inline const auto SET_LINE_SPACING                              =       sequence::controlSequence(1, table::finalWithIntermediate::SLS);
                    inline const auto SELECT_PRESENTATION_DIRECTIONS                =       sequence::controlSequence(2, table::finalWithIntermediate::SPD);
                    // inline const auto SET_PAGE_HOME                                 =       sequence::controlSequence(1, table::finalWithIntermediate::SPH); // Ecma lists these, but there are no mentions in the tables.
                    inline const auto SPACING_INCREMENT                             =       sequence::controlSequence(2, table::finalWithIntermediate::SPI);
                    // inline const auto SET_PAGE_LIMIT                                =       sequence::controlSequence(1, table::finalWithIntermediate::SPL); // Ecma lists these, but there are no mentions in the tables.
                    inline const auto SELECT_PRINT_QUALITY_AND_RAPIDITY             =       sequence::controlSequence(1, table::finalWithIntermediate::SPQR);
                    inline const auto SET_REDUCED_CHARACTER_SEPARATION              =       sequence::controlSequence(1, table::finalWithIntermediate::SRCS);
                    inline const auto START_REVERSED_STRING                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::SRS);
                    inline const auto SELECT_SIZE_UNIT                              =       sequence::controlSequence(1, table::finalWithIntermediate::SSU);
                    inline const auto SELECT_SPACE_WIDTH                            =       sequence::controlSequence(1, table::finalWithIntermediate::SSW);
                    inline const auto SELECTIVE_TABULATION                          =       sequence::controlSequence(1, table::finalWithIntermediate::STAB);
                    inline const auto SELECT_LINE_SPACING                           =       sequence::controlSequence(1, table::finalWithIntermediate::SVS);
                    inline const auto TABULATION_ALIGNED_CENTRED                    =       sequence::controlSequence(1, table::finalWithIntermediate::TAC);
                    inline const auto TABULATION_ALIGNED_LEADING_EDGE               =       sequence::controlSequence(1, table::finalWithIntermediate::TALE);
                    inline const auto TABULATION_ALIGNED_TRAILING_EDGE              =       sequence::controlSequence(1, table::finalWithIntermediate::TATE);
                    inline const auto TABULATION_CENTRED_ON_CHARACTER               =       sequence::controlSequence(2, table::finalWithIntermediate::TCC);
                    inline const auto THIN_SPACE_SPECIFICATION                      =       sequence::controlSequence(1, table::finalWithIntermediate::TSS);
                }

                namespace editorFunctions {
                    inline const auto DELETE_CHARACTER                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::DCH);
                    inline const auto DELETE_LINE                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::DL);
                    inline const auto ERASE_IN_AREA                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::EA);
                    inline const auto ERASE_CHARACTER                               =       sequence::controlSequence(1, table::finalWithoutIntermediate::ECH);
                    inline const auto ERASE_IN_PAGE                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::ED);
                    inline const auto ERASE_IN_FIELD                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::EF);
                    inline const auto ERASE_IN_LINE                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::EL);
                    inline const auto INSERT_CHARACTER                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::ICH);
                    inline const auto INSERT_LINE                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::IL);
                }

                namespace cursorControlFunctions {
                    inline const auto CURSOR_BACKWARD_TABULATION                    =       sequence::controlSequence(1, table::finalWithoutIntermediate::CBT);
                    inline const auto CURSOR_CHARACTER_ABSOLUTE                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CHA);
                    inline const auto CURSOR_FORWARD_TABULATION                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CHT);
                    inline const auto CURSOR_NEXT_LINE                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::CNL);
                    inline const auto CURSOR_PRECEDING_LINE                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::CPL);
                    inline const auto CURSOR_TABULATION_CONTROL                     =       sequence::controlSequence(-1, table::finalWithoutIntermediate::CTC);
                    inline const auto CURSOR_LEFT                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUB);
                    inline const auto CURSOR_DOWN                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUD);
                    inline const auto CURSOR_RIGHT                                  =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUF);
                    inline const auto CURSOR_POSITION                               =       sequence::controlSequence(2, table::finalWithoutIntermediate::CUP);
                    inline const auto CURSOR_UP                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUU);
                    inline const auto CURSOR_LINE_TABULATION                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::CVT);
                }

                namespace displayControlFunctions {
                    inline const auto NEXT_PAGE                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::NP);
                    inline const auto PRECEDING_PAGE                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::PP);
                    inline const auto SCROLL_DOWN                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::SD);
                    inline const auto SCROLL_LEFT                                   =       sequence::controlSequence(1, table::finalWithIntermediate::SL);
                    inline const auto SCROLL_RIGHT                                  =       sequence::controlSequence(1, table::finalWithIntermediate::SR);
                    inline const auto SCROLL_UP                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::SU);
                }

                namespace deviceControlFunctions {
                    inline const auto DEVICE_CONTROL_ONE                            =       sequence::basic(table::C0::DC1);
                    inline const auto DEVICE_CONTROL_TWO                            =       sequence::basic(table::C0::DC2);
                    inline const auto DEVICE_CONTROL_THREE                          =       sequence::basic(table::C0::DC3);
                    inline const auto DEVICE_CONTROL_FOUR                           =       sequence::basic(table::C0::DC4);
                }

                namespace informationSeparators {
                    inline const auto INFORMATION_SEPARATOR_ONE                     =       sequence::basic(table::C0::IS1);
                    inline const auto INFORMATION_SEPARATOR_TWO                     =       sequence::basic(table::C0::IS2);
                    inline const auto INFORMATION_SEPARATOR_THREE                   =       sequence::basic(table::C0::IS3);
                    inline const auto INFORMATION_SEPARATOR_FOUR                    =       sequence::basic(table::C0::IS4);
                }

                namespace areaDefinitions {
                    inline const auto DEFINE_AREA_QUALIFICATION                     =       sequence::controlSequence(-1, table::finalWithoutIntermediate::DAQ);
                    inline const auto END_OF_GUARDED_AREA                           =       sequence::basic(table::C1::EPA);
                    inline const auto END_OF_SELECTED_AREA                          =       sequence::basic(table::C1::ESA);
                    inline const auto START_OF_GUARDED_AREA                         =       sequence::basic(table::C1::SPA);
                    inline const auto START_OF_SELECTED_AREA                        =       sequence::basic(table::C1::SSA);
                }

                namespace modeSettings {
                    inline const auto RESET_MODE                                  =       sequence::controlSequence(-1, table::finalWithoutIntermediate::RM);
                    inline const auto SET_MODE                                    =       sequence::controlSequence(-1, table::finalWithoutIntermediate::SM);
                }

                namespace transmissionControlFunctions {
                    inline const auto ACKNOWLEDGE                                   =       sequence::basic(table::C0::ACK);
                    inline const auto DATA_LINK_ESCAPE                              =       sequence::basic(table::C0::DLE);
                    inline const auto ENQUIRY                                       =       sequence::basic(table::C0::ENQ);
                    inline const auto END_OF_TRANSMISSION                           =       sequence::basic(table::C0::EOT);
                    inline const auto END_OF_TRANSMISSION_BLOCK                     =       sequence::basic(table::C0::ETB);
                    inline const auto END_OF_TEXT                                   =       sequence::basic(table::C0::ETX);
                    inline const auto NEGATIVE_ACKNOWLEDGE                          =       sequence::basic(table::C0::NAK);
                    inline const auto START_OF_HEADING                              =       sequence::basic(table::C0::SOH);
                    inline const auto START_OF_TEXT                                 =       sequence::basic(table::C0::STX);
                    inline const auto SYNCHRONOUS_IDLE                              =       sequence::basic(table::C0::SYN);
                }

                namespace miscellaneousControlFunctions {
                    inline const auto BELL                                          =       sequence::basic(table::C0::BEL);
                    inline const auto CANCEL                                        =       sequence::basic(table::C0::CAN);
                    inline const auto CANCEL_CHARACTER                              =       sequence::basic(table::C1::CCH);
                    inline const auto ACTIVE_POSITION_REPORT                        =       sequence::controlSequence(2, table::finalWithoutIntermediate::CPR);
                    inline const auto DEVICE_ATTRIBUTES                             =       sequence::controlSequence(1, table::finalWithoutIntermediate::DA);
                    inline const auto DISABLE_MANUAL_INPUT                          =       sequence::independent(table::independentFunctions::DMI);
                    inline const auto DEVICE_STATUS_REPORT                          =       sequence::controlSequence(1, table::finalWithoutIntermediate::DSR);
                    inline const auto END_OF_MEDIUM                                 =       sequence::basic(table::C0::EM);
                    inline const auto ENABLE_MANUAL_INPUT                           =       sequence::independent(table::independentFunctions::EMI);
                    inline const auto FUNCTION_KEY                                  =       sequence::controlSequence(1, table::finalWithIntermediate::FNK);
                    inline const auto IDENTIFY_DEVICE_CONTROL_STRING                =       sequence::controlSequence(1, table::finalWithIntermediate::IDCS);
                    inline const auto IDENTIFY_GRAPHIC_SUBREPERTOIRE                =       sequence::controlSequence(1, table::finalWithIntermediate::IGS);
                    inline const auto INTERRUPT                                     =       sequence::independent(table::independentFunctions::INT);
                    inline const auto MEDIA_COPY                                    =       sequence::controlSequence(1, table::finalWithoutIntermediate::MC);
                    inline const auto MESSAGE_WAITING                               =       sequence::basic(table::C1::MW);
                    inline const auto NULL_CHARACTER                                =       sequence::basic(table::C0::NUL);
                    inline const auto PRIVATE_USE_ONE                               =       sequence::basic(table::C1::PU1);
                    inline const auto PRIVATE_USE_TWO                               =       sequence::basic(table::C1::PU2);
                    inline const auto REPEAT                                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::REP);
                    inline const auto RESET_TO_INITIAL_STATE                        =       sequence::independent(table::independentFunctions::RIS);
                    inline const auto SELECT_EDITING_EXTENT                         =       sequence::controlSequence(2, table::finalWithoutIntermediate::SSE);
                    inline const auto SHEET_EJECT_AND_FEED                          =       sequence::controlSequence(2, table::finalWithIntermediate::SEF);
                    inline const auto SET_TRANSMIT_STATE                            =       sequence::basic(table::C1::STS);
                    inline const auto SUBSTITUTE                                    =       sequence::basic(table::C0::SUB);
                }
            }

        }
    }
}

#endif