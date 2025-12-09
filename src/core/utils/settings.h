#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <chrono>
#include <string>
#include <vector>
#include <functional>

namespace GGUI {
    namespace INTERNAL {
        extern std::string constructLoggerFileName();
    }

    namespace SETTINGS{
        /**
         * @brief Enumeration of supported argument types for command line parsing.
         */
        enum class argumentType {
            FLAG,           ///< Boolean flag argument (no value expected)
            STRING,         ///< String value argument
            INTEGER,        ///< Integer value argument
            UNSIGNED_LONG   ///< Unsigned long value argument
        };

        /**
         * @brief Descriptor class for command line arguments.
         * 
         * This class encapsulates information about a command line argument including
         * its name, type, description, and a callback function to handle the parsed value.
         */
        class argumentDescriptor {
        public:
            std::string name;                                    ///< Argument name (without dashes)
            argumentType type;                                   ///< Type of argument (flag, string, integer, etc.)
            std::string description;                             ///< Human-readable description for help text
            std::function<void(const std::string&)> handler;    ///< Callback function to handle parsed value

            /**
             * @brief Constructs an ArgumentDescriptor.
             * 
             * @param argName The name of the argument (without leading dashes)
             * @param argType The type of the argument
             * @param argDescription Description of the argument for help text
             * @param argHandler Callback function to handle the parsed value
             */
            argumentDescriptor(const std::string& argName, 
                             argumentType argType, 
                             const std::string& argDescription,
                             std::function<void(const std::string&)> argHandler)
                : name(argName), type(argType), description(argDescription), handler(argHandler) {}

            /**
             * @brief Checks if this argument requires a value.
             * 
             * @return true if the argument type requires a value, false for flags
             */
            bool requiresvalue() const {
                return type != argumentType::FLAG;
            }

            /**
             * @brief Gets the type name as a string for help text.
             * 
             * @return String representation of the argument type
             */
            std::string getTypeName() const {
                switch (type) {
                    case argumentType::FLAG: return "flag";
                    case argumentType::STRING: return "string";
                    case argumentType::INTEGER: return "integer";
                    case argumentType::UNSIGNED_LONG: return "unsigned long";
                    default: return "unknown";
                }
            }
        };

        // Given as --mousePressCooldown = 123
        extern unsigned long long mousePressDownCooldown;  // Milliseconds

        // Given as --enableWordWrapping
        extern bool wordWrapping;

        // Given as --enableGammaCorrection
        extern bool enableGammaCorrection;

        namespace LOGGER{
            // Given as --loggerFileName = "GGUI.log"
            extern std::string fileName;
        }

        // Given as --enableDRM
        extern bool enableDRM;

        extern void parseCommandLineArguments(int argc, char** argv);

        /**
         * @brief Initializes the settings for the application.
         *
         * This function sets up the necessary configurations for the application
         * by initializing the logger file name using the internal logger file name
         * construction method.
         */
        extern void initSettings();
    }
}

#endif