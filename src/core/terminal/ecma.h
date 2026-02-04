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
                template<typename E>
                bool contains(auto val) {
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

                    parameter(std::string_view input, int& length);

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
                    basic(std::string_view input, int& length);
                    basic(table::C0 func) : base(specialType::BASIC, bitType::_7bit), function(func) {}
                    basic(table::C1 func) : base(specialType::BASIC, bitType::_8bit), function(func) {}

                    std::string toString() override;
                };

                class controlSequence : public base {
                protected:
                    int parameterCount = 0;                     // If -1, then no constraints.
                    int intermediateCount = 0;                  // if 1 and no specified, defaults into 02/00 (' ')
                    std::vector<parameter> parameters;          // Each range between: 03/00 - 03/15, delimeetered by 03/11 (';')
                    std::vector<uint8_t> intermediates;         // Each range between: 02/00 - 02/15
                    std::variant<
                        table::finalWithoutIntermediate,        // Each range between: 04/00 - 07/14
                        table::finalWithIntermediate            // Each range between: 04/00 - 07/14
                    > finalByte;
                public:
                    controlSequence(std::string_view input, int& length);

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
                        int maxParamCount,
                        table::finalWithoutIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameterCount(maxParamCount), finalByte(finalByte) {}

                    controlSequence(
                        int maxParamCount,
                        table::finalWithIntermediate finalByte
                    ) : base(specialType::CONTROL_SEQUENCE, bitType::_7bit), parameterCount(maxParamCount), intermediateCount(1), intermediates({table::toInt(2, 0)}), finalByte(finalByte) {}

                    std::string toString() override;
                };

                class independent : public base {
                protected:
                    bool hasSpace = false;                      // Can be: 02/03 (' ')
                    table::independentFunctions function;
                public:
                    independent(std::string_view input, int& length);

                    independent(
                        table::independentFunctions func,
                        bool space = false
                    ) : base(specialType::INDEPENDENT, bitType::_7bit), function(func), hasSpace(space) {}

                    std::string toString() override;
                };

                class controlString : public base {
                protected:
                    table::C1 openingDelimiter;                 // APC, DCS, OSC, PM or SOS
                    std::vector<basic*> commands;               // In the range 00/08 to 00/13 and 02/00 to 07/14
                    std::vector<uint8_t> characters;            // For when SOS is used in the opening delimeter, can contain in range of 00/00 to 07/15
                public:
                    controlString(std::string_view input, int& length);

                    controlString(
                        table::C1 delimiter,
                        std::vector<basic*> cmds
                    ) : base(specialType::CONTROL_STRING, bitType::_7bit), openingDelimiter(delimiter), commands(cmds) {}

                    controlString(
                        std::vector<uint8_t> chars
                    ) : base(specialType::CONTROL_STRING, bitType::_7bit), openingDelimiter(table::C1::SOS), characters(chars) {}

                    std::string toString() override;
                };
            
                std::vector<base*> parse(std::string_view input);
            }

            namespace sequences {

                namespace delimiters {
                    static const auto APPLICATION_PROGRAM_COMMAND                   =       sequence::basic(table::C1::APC);
                    static const auto CODING_METHOD_DELIMITER                       =       sequence::independent(table::independentFunctions::CMD);
                    static const auto DEVICE_CONTROL_STRING                         =       sequence::basic(table::C1::DCS);
                    static const auto OPERATING_SYSTEM_COMMAND                      =       sequence::basic(table::C1::OSC);
                    static const auto PRIVACY_MESSAGE                               =       sequence::basic(table::C1::PM);
                    static const auto START_OF_STRING                               =       sequence::basic(table::C1::SOS);
                    static const auto STRING_TERMINATOR                             =       sequence::basic(table::C1::ST);
                };

                namespace introducers {
                    static const auto CONTROL_SEQUENCE_INTRODUCER                   =       sequence::basic(table::C1::CSI);
                    static const auto ESCAPE                                        =       sequence::basic(table::C0::ESC);
                    static const auto SINGLE_CHARACTER_INTRODUCER                   =       sequence::basic(table::C1::SCI);
                }

                namespace shiftFunctions {
                    static const auto LOCKING_SHIFT_ZERO                            =       sequence::basic(table::C0::LS0);
                    static const auto LOCKING_SHIFT_ONE                             =       sequence::basic(table::C0::LS1);
                    static const auto LOCKING_SHIFT_ONE_RIGHT                       =       sequence::independent(table::independentFunctions::LS1R);
                    static const auto LOCKING_SHIFT_TWO                             =       sequence::independent(table::independentFunctions::LS2);
                    static const auto LOCKING_SHIFT_TWO_RIGHT                       =       sequence::independent(table::independentFunctions::LS2R);
                    static const auto LOCKING_SHIFT_THREE                           =       sequence::independent(table::independentFunctions::LS3);
                    static const auto LOCKING_SHIFT_THREE_RIGHT                     =       sequence::independent(table::independentFunctions::LS3R);
                    static const auto SHIFT_IN                                      =       sequence::basic(table::C0::SI);
                    static const auto SHIFT_OUT                                     =       sequence::basic(table::C0::SO);
                    static const auto SS2                                           =       sequence::basic(table::C1::SS2);
                    static const auto SS3                                           =       sequence::basic(table::C1::SS3);
                }

                namespace formatEffectors {
                    static const auto BACKSPACE                                     =       sequence::basic(table::C0::BS);
                    static const auto CARRIAGE_RETURN                               =       sequence::basic(table::C0::CR);
                    static const auto FORM_FEED                                     =       sequence::basic(table::C0::FF);
                    static const auto CHARACTER_POSITION_ABSOLUTE                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::HPA);
                    static const auto CHARACTER_POSITION_BACKWARD                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::HPB);
                    static const auto CHARACTER_POSITION_FORWARD                    =       sequence::controlSequence(1, table::finalWithoutIntermediate::HPR);
                    static const auto CHARACTER_TABULATION                          =       sequence::basic(table::C0::HT);
                    static const auto CHARACTER_TABULATION_WITH_JUSTIFICATION       =       sequence::basic(table::C1::HTJ);
                    static const auto CHARACTER_TABULATION_SET                      =       sequence::basic(table::C1::HTS);
                    static const auto CHARACTER_AND_LINE_POSITION                   =       sequence::controlSequence(2, table::finalWithoutIntermediate::HVP);
                    static const auto LINE_FEED                                     =       sequence::basic(table::C0::LF);
                    static const auto NEXT_LINE                                     =       sequence::basic(table::C1::NEL);
                    static const auto PARTIAL_LINE_FORWARD                          =       sequence::basic(table::C1::PLD);
                    static const auto PARTIAL_LINE_BACKWARD                         =       sequence::basic(table::C1::PLU);
                    static const auto PAGE_POSITION_ABSOLUTE                        =       sequence::controlSequence(1, table::finalWithIntermediate::PPA);
                    static const auto PAGE_POSITION_BACKWARD                        =       sequence::controlSequence(1, table::finalWithIntermediate::PPB);
                    static const auto PAGE_POSITION_FORWARD                         =       sequence::controlSequence(1, table::finalWithIntermediate::PPR);
                    static const auto REVERSE_LINE_FEED                             =       sequence::basic(table::C1::RI);
                    static const auto TABULATION_CLEAR                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::TBC);
                    static const auto TABULATION_STOP_REMOVE                        =       sequence::controlSequence(1, table::finalWithIntermediate::TSR);
                    static const auto LINE_POSITION_ABSOLUTE                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::VPA);
                    static const auto LINE_POSITION_BACKWARD                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::VPB);
                    static const auto LINE_POSITION_FORWARD                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::VPR);
                    static const auto LINE_TABULATION                               =       sequence::basic(table::C0::VT);
                    static const auto LINE_TABULATION_SET                           =       sequence::basic(table::C1::VTS);
                }

                namespace presentationControlFunctions {
                    static const auto BREAK_PERMITTED_HERE                          =       sequence::basic(table::C1::BPH);
                    static const auto DIMENSION_TEXT_AREA                           =       sequence::controlSequence(2, table::finalWithIntermediate::DTA);
                    static const auto FONT_SELECTION                                =       sequence::controlSequence(2, table::finalWithIntermediate::FNT);
                    static const auto GRAPHIC_CHARACTER_COMBINATION                 =       sequence::controlSequence(1, table::finalWithIntermediate::GCC);
                    static const auto GRAPHIC_SIZE_MODIFICATION                     =       sequence::controlSequence(2, table::finalWithIntermediate::GSM);
                    static const auto GRAPHIC_SIZE_SELECTION                        =       sequence::controlSequence(1, table::finalWithIntermediate::GSS);
                    static const auto JUSTIFY                                       =       sequence::controlSequence(-1, table::finalWithIntermediate::JFY);
                    static const auto NO_BREAK_HERE                                 =       sequence::basic(table::C1::NBH);
                    static const auto PRESENTATION_EXPAND_OR_CONTRACT               =       sequence::controlSequence(1, table::finalWithIntermediate::PEC);
                    static const auto PAGE_FORMAT_SELECTION                         =       sequence::controlSequence(1, table::finalWithIntermediate::PFS);
                    static const auto PARALLEL_TEXTS                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::PTX);
                    static const auto QUAD                                          =       sequence::controlSequence(-1, table::finalWithIntermediate::QUAD);
                    static const auto SET_ADDITIONAL_CHARACTER_SEPARATION           =       sequence::controlSequence(1, table::finalWithIntermediate::SACS);
                    static const auto SELECT_ALTERNATIVE_PRESENTATION_VARIANTS      =       sequence::controlSequence(-1, table::finalWithIntermediate::SAPV);
                    static const auto SET_CHARACTER_ORIENTATION                     =       sequence::controlSequence(1, table::finalWithIntermediate::SCO);
                    static const auto SELECT_CHARACTER_PATH                         =       sequence::controlSequence(2, table::finalWithIntermediate::SCP);
                    static const auto SET_CHARACTER_SPACING                         =       sequence::controlSequence(1, table::finalWithIntermediate::SCS);
                    static const auto START_DIRECTED_STRING                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::SDS);
                    static const auto SELECT_GRAPHIC_RENDITION                      =       sequence::controlSequence(-1, table::finalWithoutIntermediate::SGR);
                    static const auto SELECT_CHARACTER_SPACING                      =       sequence::controlSequence(1, table::finalWithIntermediate::SHS);
                    static const auto SELECT_IMPLICIT_MOVEMENT_DIRECTION            =       sequence::controlSequence(1, table::finalWithoutIntermediate::SIMD);
                    static const auto SET_LINE_HOME                                 =       sequence::controlSequence(1, table::finalWithIntermediate::SHL);
                    static const auto SET_LINE_LIMIT                                =       sequence::controlSequence(1, table::finalWithIntermediate::SLL);
                    static const auto SET_LINE_SPACING                              =       sequence::controlSequence(1, table::finalWithIntermediate::SLS);
                    static const auto SELECT_PRESENTATION_DIRECTIONS                =       sequence::controlSequence(2, table::finalWithIntermediate::SPD);
                    // static const auto SET_PAGE_HOME                                 =       sequence::controlSequence(1, table::finalWithIntermediate::SPH); // Ecma lists these, but there are no mentions in the tables.
                    static const auto SPACING_INCREMENT                             =       sequence::controlSequence(2, table::finalWithIntermediate::SPI);
                    // static const auto SET_PAGE_LIMIT                                =       sequence::controlSequence(1, table::finalWithIntermediate::SPL); // Ecma lists these, but there are no mentions in the tables.
                    static const auto SELECT_PRINT_QUALITY_AND_RAPIDITY             =       sequence::controlSequence(1, table::finalWithIntermediate::SPQR);
                    static const auto SET_REDUCED_CHARACTER_SEPARATION              =       sequence::controlSequence(1, table::finalWithIntermediate::SRCS);
                    static const auto START_REVERSED_STRING                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::SRS);
                    static const auto SELECT_SIZE_UNIT                              =       sequence::controlSequence(1, table::finalWithIntermediate::SSU);
                    static const auto SELECT_SPACE_WIDTH                            =       sequence::controlSequence(1, table::finalWithIntermediate::SSW);
                    static const auto SELECTIVE_TABULATION                          =       sequence::controlSequence(1, table::finalWithIntermediate::STAB);
                    static const auto SELECT_LINE_SPACING                           =       sequence::controlSequence(1, table::finalWithIntermediate::SVS);
                    static const auto TABULATION_ALIGNED_CENTRED                    =       sequence::controlSequence(1, table::finalWithIntermediate::TAC);
                    static const auto TABULATION_ALIGNED_LEADING_EDGE               =       sequence::controlSequence(1, table::finalWithIntermediate::TALE);
                    static const auto TABULATION_ALIGNED_TRAILING_EDGE              =       sequence::controlSequence(1, table::finalWithIntermediate::TATE);
                    static const auto TABULATION_CENTRED_ON_CHARACTER               =       sequence::controlSequence(2, table::finalWithIntermediate::TCC);
                    static const auto THIN_SPACE_SPECIFICATION                      =       sequence::controlSequence(1, table::finalWithIntermediate::TSS);
                }

                namespace editorFunctions {
                    static const auto DELETE_CHARACTER                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::DCH);
                    static const auto DELETE_LINE                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::DL);
                    static const auto ERASE_IN_AREA                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::EA);
                    static const auto ERASE_CHARACTER                               =       sequence::controlSequence(1, table::finalWithoutIntermediate::ECH);
                    static const auto ERASE_IN_PAGE                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::ED);
                    static const auto ERASE_IN_FIELD                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::EF);
                    static const auto ERASE_IN_LINE                                 =       sequence::controlSequence(1, table::finalWithoutIntermediate::EL);
                    static const auto INSERT_CHARACTER                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::ICH);
                    static const auto INSERT_LINE                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::IL);
                }

                namespace cursorControlFunctions {
                    static const auto CURSOR_BACKWARD_TABULATION                    =       sequence::controlSequence(1, table::finalWithoutIntermediate::CBT);
                    static const auto CURSOR_CHARACTER_ABSOLUTE                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CHA);
                    static const auto CURSOR_FORWARD_TABULATION                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CHT);
                    static const auto CURSOR_NEXT_LINE                              =       sequence::controlSequence(1, table::finalWithoutIntermediate::CNL);
                    static const auto CURSOR_PRECEDING_LINE                         =       sequence::controlSequence(1, table::finalWithoutIntermediate::CPL);
                    static const auto CURSOR_TABULATION_CONTROL                     =       sequence::controlSequence(-1, table::finalWithoutIntermediate::CTC);
                    static const auto CURSOR_LEFT                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUB);
                    static const auto CURSOR_DOWN                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUD);
                    static const auto CURSOR_RIGHT                                  =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUF);
                    static const auto CURSOR_POSITION                               =       sequence::controlSequence(2, table::finalWithoutIntermediate::CUP);
                    static const auto CURSOR_UP                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::CUU);
                    static const auto CURSOR_LINE_TABULATION                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::CVT);
                }

                namespace displayControlFunctions {
                    static const auto NEXT_PAGE                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::NP);
                    static const auto PRECEDING_PAGE                                =       sequence::controlSequence(1, table::finalWithoutIntermediate::PP);
                    static const auto SCROLL_DOWN                                   =       sequence::controlSequence(1, table::finalWithoutIntermediate::SD);
                    static const auto SCROLL_LEFT                                   =       sequence::controlSequence(1, table::finalWithIntermediate::SL);
                    static const auto SCROLL_RIGHT                                  =       sequence::controlSequence(1, table::finalWithIntermediate::SR);
                    static const auto SCROLL_UP                                     =       sequence::controlSequence(1, table::finalWithoutIntermediate::SU);
                }

                namespace deviceControlFunctions {
                    static const auto DEVICE_CONTROL_ONE                            =       sequence::basic(table::C0::DC1);
                    static const auto DEVICE_CONTROL_TWO                            =       sequence::basic(table::C0::DC2);
                    static const auto DEVICE_CONTROL_THREE                          =       sequence::basic(table::C0::DC3);
                    static const auto DEVICE_CONTROL_FOUR                           =       sequence::basic(table::C0::DC4);
                }

                namespace informationSeparators {
                    static const auto INFORMATION_SEPARATOR_ONE                     =       sequence::basic(table::C0::IS1);
                    static const auto INFORMATION_SEPARATOR_TWO                     =       sequence::basic(table::C0::IS2);
                    static const auto INFORMATION_SEPARATOR_THREE                   =       sequence::basic(table::C0::IS3);
                    static const auto INFORMATION_SEPARATOR_FOUR                    =       sequence::basic(table::C0::IS4);
                }

                namespace areaDefinitions {
                    static const auto DEFINE_AREA_QUALIFICATION                     =       sequence::controlSequence(-1, table::finalWithoutIntermediate::DAQ);
                    static const auto END_OF_GUARDED_AREA                           =       sequence::basic(table::C1::EPA);
                    static const auto END_OF_SELECTED_AREA                          =       sequence::basic(table::C1::ESA);
                    static const auto START_OF_GUARDED_AREA                         =       sequence::basic(table::C1::SPA);
                    static const auto START_OF_SELECTED_AREA                        =       sequence::basic(table::C1::SSA);
                }

                namespace modeSettings {
                    static const auto RESET_MODE                                  =       sequence::controlSequence(-1, table::finalWithoutIntermediate::RM);
                    static const auto SET_MODE                                    =       sequence::controlSequence(-1, table::finalWithoutIntermediate::SM);
                }

                namespace transmissionControlFunctions {
                    static const auto ACKNOWLEDGE                                   =       sequence::basic(table::C0::ACK);
                    static const auto DATA_LINK_ESCAPE                              =       sequence::basic(table::C0::DLE);
                    static const auto ENQUIRY                                       =       sequence::basic(table::C0::ENQ);
                    static const auto END_OF_TRANSMISSION                           =       sequence::basic(table::C0::EOT);
                    static const auto END_OF_TRANSMISSION_BLOCK                     =       sequence::basic(table::C0::ETB);
                    static const auto END_OF_TEXT                                   =       sequence::basic(table::C0::ETX);
                    static const auto NEGATIVE_ACKNOWLEDGE                          =       sequence::basic(table::C0::NAK);
                    static const auto START_OF_HEADING                              =       sequence::basic(table::C0::SOH);
                    static const auto START_OF_TEXT                                 =       sequence::basic(table::C0::STX);
                    static const auto SYNCHRONOUS_IDLE                              =       sequence::basic(table::C0::SYN);
                }

                namespace miscellaneousControlFunctions {
                    static const auto BELL                                          =       sequence::basic(table::C0::BEL);
                    static const auto CANCEL                                        =       sequence::basic(table::C0::CAN);
                    static const auto CANCEL_CHARACTER                              =       sequence::basic(table::C1::CCH);
                    static const auto ACTIVE_POSITION_REPORT                        =       sequence::controlSequence(2, table::finalWithoutIntermediate::CPR);
                    static const auto DEVICE_ATTRIBUTES                             =       sequence::controlSequence(1, table::finalWithoutIntermediate::DA);
                    static const auto DISABLE_MANUAL_INPUT                          =       sequence::independent(table::independentFunctions::DMI);
                    static const auto DEVICE_STATUS_REPORT                          =       sequence::controlSequence(1, table::finalWithoutIntermediate::DSR);
                    static const auto END_OF_MEDIUM                                 =       sequence::basic(table::C0::EM);
                    static const auto ENABLE_MANUAL_INPUT                           =       sequence::independent(table::independentFunctions::EMI);
                    static const auto FUNCTION_KEY                                  =       sequence::controlSequence(1, table::finalWithIntermediate::FNK);
                    static const auto IDENTIFY_DEVICE_CONTROL_STRING                =       sequence::controlSequence(1, table::finalWithIntermediate::IDCS);
                    static const auto IDENTIFY_GRAPHIC_SUBREPERTOIRE                =       sequence::controlSequence(1, table::finalWithIntermediate::IGS);
                    static const auto INTERRUPT                                     =       sequence::independent(table::independentFunctions::INT);
                    static const auto MEDIA_COPY                                    =       sequence::controlSequence(1, table::finalWithoutIntermediate::MC);
                    static const auto MESSAGE_WAITING                               =       sequence::basic(table::C1::MW);
                    static const auto NULL_CHARACTER                                =       sequence::basic(table::C0::NUL);
                    static const auto PRIVATE_USE_ONE                               =       sequence::basic(table::C1::PU1);
                    static const auto PRIVATE_USE_TWO                               =       sequence::basic(table::C1::PU2);
                    static const auto REPEAT                                        =       sequence::controlSequence(1, table::finalWithoutIntermediate::REP);
                    static const auto RESET_TO_INITIAL_STATE                        =       sequence::independent(table::independentFunctions::RIS);
                    static const auto SELECT_EDITING_EXTENT                         =       sequence::controlSequence(2, table::finalWithoutIntermediate::SSE);
                    static const auto SHEET_EJECT_AND_FEED                          =       sequence::controlSequence(2, table::finalWithIntermediate::SEF);
                    static const auto SET_TRANSMIT_STATE                            =       sequence::basic(table::C1::STS);
                    static const auto SUBSTITUTE                                    =       sequence::basic(table::C0::SUB);
                }
            }

        }
    }
}

#endif