#include "settings.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cctype>

#include "logger.h"

namespace GGUI {
    namespace SETTINGS {
        
        // Define the actual storage for the settings variables with their default values
        unsigned long long mousePressDownCooldown = 365;  // Milliseconds
        bool wordWrapping = true;
        bool enableGammaCorrection = false;
        bool enableDRM = false;
        
        namespace LOGGER {
            std::string fileName = "";
        }

        static std::string toLower(const std::string& in) {
            std::string out = in;
            std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
            return out;
        }

        /**
         * @brief Removes leading dashes from an argument string.
         * 
         * This function removes any number of leading dashes from the argument,
         * allowing users to use single dash (-), double dash (--), or no dashes at all.
         * 
         * @param arg The argument string to process
         * @return The argument string with leading dashes removed
         */
        static std::string removeDashes(const std::string& arg) {
            size_t start = 0;
            while (start < arg.length() && arg[start] == '-') {
                start++;
            }
            return arg.substr(start);
        }

        /**
         * @brief Splits command line arguments by spaces and handles quoted strings.
         * 
         * This function properly handles quoted strings and splits the command line
         * into individual tokens while preserving quoted content.
         * 
         * @param argc Number of command line arguments
         * @param argv Array of command line argument strings
         * @return Vector of processed argument tokens
         */
        static std::vector<std::string> tokenizeArguments(int argc, char** argv) {
            std::vector<std::string> tokens;
            
            for (int i = 1; i < argc; i++) {
                std::string arg = argv[i];
                
                // Handle quoted arguments that might be split across multiple argv entries
                if (!arg.empty() && arg[0] == '"' && arg.back() != '"') {
                    // Start of quoted string, collect until closing quote
                    while (++i < argc) {
                        arg += " " + std::string(argv[i]);
                        if (argv[i][strlen(argv[i]) - 1] == '"') {
                            break;
                        }
                    }
                }
                
                tokens.push_back(arg);
            }
            
            return tokens;
        }

        /**
         * @brief Displays help information for all available command line arguments.
         * 
         * @param descriptors Vector of argument descriptors to display help for
         */
        static void displayHelp(const std::vector<argumentDescriptor>& descriptors) {
            INTERNAL::LOGGER::log("GGUI - Professional Command Line Interface\n");
            INTERNAL::LOGGER::log("==========================================\n\n");
            INTERNAL::LOGGER::log("Usage: GGUI [OPTIONS]\n\n");
            INTERNAL::LOGGER::log("Available Options:\n");
            
            for (const auto& desc : descriptors) {
                INTERNAL::LOGGER::log("  --" + desc.name);
                
                if (desc.requiresvalue()) {
                    INTERNAL::LOGGER::log("=<" + desc.getTypeName() + ">");
                }
                
                // Align descriptions
                std::string spacing(std::max(1, 30 - static_cast<int>(desc.name.length() + 
                    (desc.requiresvalue() ? desc.getTypeName().length() + 3 : 0))), ' ');
                INTERNAL::LOGGER::log(spacing + desc.description + "\n");
            }
            
            INTERNAL::LOGGER::log("\nExamples:\n");
            INTERNAL::LOGGER::log("  GGUI --enableDRM --mousePressCooldown=500\n");
            INTERNAL::LOGGER::log("  GGUI -enableGammaCorrection --loggerFileName=\"debug.log\"\n");
            INTERNAL::LOGGER::log("  GGUI enableWordWrapping mousePressCooldown=1000\n\n");
        }

        void parseCommandLineArguments(int argc, char** argv) {
            // Define argument descriptors with their handlers
            std::vector<argumentDescriptor> argumentDescriptors = {
                argumentDescriptor(
                    "mousePressCooldown",
                    argumentType::UNSIGNED_LONG,
                    "Set mouse press cooldown in milliseconds (default: 365)",
                    [](const std::string& value) {
                        try {
                            mousePressDownCooldown = std::stoull(value);
                        } catch (const std::exception& e) {
                            INTERNAL::LOGGER::log("Error: Invalid value for mousePressCooldown: " + value);
                            INTERNAL::LOGGER::log("Expected an unsigned integer value.");
                        }
                    }
                ),
                
                argumentDescriptor(
                    "enableWordWrapping",
                    argumentType::FLAG,
                    "Enable word wrapping (default: true)",
                    [](const std::string&) {
                        wordWrapping = true;
                    }
                ),
                
                argumentDescriptor(
                    "enableGammaCorrection",
                    argumentType::FLAG,
                    "Enable gamma correction (default: false)",
                    [](const std::string&) {
                        enableGammaCorrection = true;
                    }
                ),
                
                argumentDescriptor(
                    "loggerFileName",
                    argumentType::STRING,
                    "Set logger file name (default: auto-generated)",
                    [](const std::string& value) {
                        std::string cleanvalue = value;
                        // Remove quotes if present
                        if (cleanvalue.length() >= 2 && cleanvalue.front() == '"' && cleanvalue.back() == '"') {
                            cleanvalue = cleanvalue.substr(1, cleanvalue.length() - 2);
                        }
                        LOGGER::fileName = cleanvalue;
                    }
                ),
                
                argumentDescriptor(
                    "enableDRM",
                    argumentType::FLAG,
                    "Enable DRM backend for hardware acceleration (default: false)",
                    [](const std::string&) {
                        enableDRM = true;
                    }
                ),
                
                argumentDescriptor(
                    "help",
                    argumentType::FLAG,
                    "Display this help message and exit",
                    [&argumentDescriptors](const std::string&) {
                        displayHelp(argumentDescriptors);
                        exit(0);
                    }
                ),
                
                argumentDescriptor(
                    "h",
                    argumentType::FLAG,
                    "Display help message (short form)",
                    [&argumentDescriptors](const std::string&) {
                        displayHelp(argumentDescriptors);
                        exit(0);
                    }
                )
            };

            // Tokenize the command line arguments
            std::vector<std::string> tokens = tokenizeArguments(argc, argv);
            
            // Process each token
            for (size_t i = 0; i < tokens.size(); i++) {
                std::string currentToken = tokens[i];
                
                // Handle argument=value format
                std::string argName, argvalue;
                size_t equalPos = currentToken.find('=');
                
                if (equalPos != std::string::npos) {
                    argName = removeDashes(currentToken.substr(0, equalPos));
                    argvalue = currentToken.substr(equalPos + 1);
                } else {
                    argName = removeDashes(currentToken);
                }

                // Find matching argument descriptor
                auto descriptorIt = std::find_if(argumentDescriptors.begin(), argumentDescriptors.end(),
                    [&argName](const argumentDescriptor& desc) {
                        return toLower(desc.name) == toLower(argName);
                    });

                if (descriptorIt != argumentDescriptors.end()) {
                    // Found matching descriptor
                    if (descriptorIt->requiresvalue()) {
                        if (equalPos != std::string::npos) {
                            // value provided with = format
                            descriptorIt->handler(argvalue);
                        } else if (i + 1 < tokens.size()) {
                            // value should be in next token
                            std::string nextToken = tokens[++i];
                            // Don't consume if next token looks like another argument
                            if (!nextToken.empty() && nextToken[0] == '-' && nextToken.length() > 1) {
                                INTERNAL::LOGGER::log("Error: Argument --" + argName + " requires a value.");
                                i--; // Back up so this token gets processed
                            } else {
                                descriptorIt->handler(nextToken);
                            }
                        } else {
                            INTERNAL::LOGGER::log("Error: Argument --" + argName + " requires a value.");
                        }
                    } else {
                        // Flag argument
                        descriptorIt->handler("");
                    }
                } else {
                    // Unknown argument
                    if (!argName.empty()) {
                        INTERNAL::LOGGER::log("Warning: Unknown argument '" + argName + "'. Use --help for available options.");
                    }
                }
            }
        }

        /**
         * @brief Initializes the settings for the application.
         *
         * This function sets up the necessary configurations for the application
         * by initializing the logger file name using the internal logger file name
         * construction method.
         */
        void initSettings(){
            if (LOGGER::fileName.empty())
                LOGGER::fileName = INTERNAL::constructLoggerFileName();
        }
    }
}