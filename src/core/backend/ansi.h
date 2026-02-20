#ifndef _ANSI_H_
#define _ANSI_H_

#include "terminal.h"
#include "ecma.h"

namespace GGUI {
    namespace terminal {
        /**
         * @brief Builds upon the ECMA-48 standard, by renames and reroutes of basically same information, just more consumer friendly codes.
         * URL: https://nvlpubs.nist.gov/nistpubs/Legacy/FIPS/fipspub1-2-1977.pdf
        */
        namespace ansi {

            template<typename source, typename sourceBaseType = std::underlying_type_t<source>>
            constexpr sourceBaseType alias(source val) {
                return static_cast<sourceBaseType>(val);
            }

            enum class table : uint8_t {
                __min = ecma::table::toInt(0, 0),    // For internal automation

                /*          COLUMN 0           */       /*          COLUMN 1           */       /*          COLUMN 2         */     /*         COLUMN 7         */
                NUL = alias(ecma::table::C0::NUL),      DLE = alias(ecma::table::C0::DLE),      SP = ecma::table::toInt(2, 00),     /*                          */
                SOH = alias(ecma::table::C0::SOH),      DC1 = alias(ecma::table::C0::DC1),      /*                           */     /*                          */
                STX = alias(ecma::table::C0::STX),      DC2 = alias(ecma::table::C0::DC2),      /*                           */     /*                          */
                ETX = alias(ecma::table::C0::ETX),      DC3 = alias(ecma::table::C0::DC3),      /*                           */     /*                          */
                EOT = alias(ecma::table::C0::EOT),      DC4 = alias(ecma::table::C0::DC4),      /*                           */     /*                          */
                ENQ = alias(ecma::table::C0::ENQ),      NAK = alias(ecma::table::C0::NAK),      /*                           */     /*                          */
                ACK = alias(ecma::table::C0::ACK),      SYN = alias(ecma::table::C0::SYN),      /*                           */     /*                          */
                BEL = alias(ecma::table::C0::BEL),      ETB = alias(ecma::table::C0::ETB),      /*                           */     /*                          */
                BS  = alias(ecma::table::C0::BS),       CAN = alias(ecma::table::C0::CAN),      /*                           */     /*                          */
                HT  = alias(ecma::table::C0::HT),       EM  = alias(ecma::table::C0::EM),       /*                           */     /*                          */
                LF  = alias(ecma::table::C0::LF),       SUB = alias(ecma::table::C0::SUB),      /*                           */     /*                          */
                VT  = alias(ecma::table::C0::VT),       ESC = alias(ecma::table::C0::ESC),      /*                           */     /*                          */
                FF  = alias(ecma::table::C0::FF),       FS  = alias(ecma::table::C0::IS4),      /*                           */     /*                          */
                CR  = alias(ecma::table::C0::CR),       GS  = alias(ecma::table::C0::IS3),      /*                           */     /*                          */
                SO  = alias(ecma::table::C0::SO),       RS  = alias(ecma::table::C0::IS2),      /*                           */     /*                          */
                SI  = alias(ecma::table::C0::SI),       US  = alias(ecma::table::C0::IS1),      /*                           */     DEL = ecma::table::toInt(7, 15),

                __max = ecma::table::toInt(7, 15)    // For internal automation
            };

            // Helpers:
            inline std::variant<table, uint8_t> get(uint8_t index) {
                if (ecma::table::contains<table>(index)) {
                    return static_cast<table>(index);
                }

                return static_cast<uint8_t>(index); // Return the index as an uint8_t if it's not a control character
            }
            
            inline std::variant<table, uint8_t> get(uint8_t row, uint8_t column) {
                // Sanitize params
                row &= 0x0F;        // Only the first 4 bits are allowed
                column &= 0x07;     // Only the first 3 bits are allowed

                uint8_t index = (row << 4) | column; // Combine row and column to get the index

                return get(index);
            };
        }
    }
}

#endif