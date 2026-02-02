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
    
                template<enum E>
                // This is used to convert between C1 7bit and 8bit variants.
                constexpr uint8_t shiftColumns(E value, columns by, bool down = false) {
                    constexpr int16_t sign = down ? -1 : 1;

                    return static_cast<uint8_t>(
                        static_cast<int16_t>(value) + (sign * static_cast<int16_t>(by) * static_cast<int16_t>(tableRows))
                    );
                }

                enum class C0 : uint8_t {   // Table 1
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
                    LS0 = toInt(0, 15),     LS1 = toInt(1, 15),     SI = toInt(0, 15),  // <-- legacy
                };

                // NOTE: This is the 7bit table from table 2a. Use the column shifter to switch between the 2a and 2b tables in C1.
                enum class C1 : uint8_t {   // Table 2
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
                };

                enum class finalWithoutIntermediate : uint8_t {   // Table 3 Final Bytes of control sequences without Intermediate Bytes 
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
                };

                enum class finalWithIntermediate : uint8_t {   // Table 4 - Final Bytes of control sequences with a single Intermediate index: (2, 0)
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
                };

                enum class independentFunctions : uint8_t { // Table 5 - Independent control functions 
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
                };
            }

            namespace sequence {
                enum class bitType {
                    _7bit,          // For the 1/11 introducers
                    _8bit           // For the 09/11 introducers
                };

                enum class specialType {
                    BASIC,                          // Contains the single-byte C0 and C1 functions
                    CONTROL,                        // CSI based functions
                    INDEPENDENT,                    // Contains ESC Fs or ESC 02/03 F functions

                };

                class parameter {
                protected:
                    std::vector<uint32_t> subNumbers;       // For instances where 1:2, these can be used as decimals.
                public:
                    constexpr static uint8_t sub_delimeter  = table::toInt(3, 10); // Translates into ':'
                    constexpr static uint8_t delimeter      = table::toInt(3, 11); // Translates into ';'

                    parameter(std::string_view input);
                    
                    std::string toString();
                };

                class base {
                public:
                    specialType type;                       // Tells what kind of function this is
                    bitType escapeType = bitType::_7bit;    // Since most terminal emulators use the 7bit introducer.

                    virtual std::string toString();
                };

                class basic : public base {
                protected:
                    std::variant<table::C0, table::C1> function;
                public:

                    basic(std::string_view input);

                    std::string toString() override;
                };

                class control : public base {
                protected:
                    std::vector<parameter> parameters;          // Each range between: 03/00 - 03/15, delimeetered by 03/11 (';')
                    std::vector<uint8_t> intermediates;         // Each range between: 02/00 - 02/15
                    std::variant<
                        table::finalWithoutIntermediate,        // Each range between: 04/00 - 07/14
                        table::finalWithIntermediate            // Each range between: 04/00 - 07/14
                    > finalByte;
                public:

                    control(std::string_view input);

                    std::string toString() override;
                };

                class independent : public base {
                protected:
                    bool hasSpace = false;                      // Can be: 02/03 (' ')
                    table::independentFunctions function;
                public:

                    independent(std::string_view input);

                    std::string toString() override;
                };
            }
        }
    }
}

#endif