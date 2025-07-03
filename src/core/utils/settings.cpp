#include "settings.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace GGUI {
    namespace SETTINGS {
        
        // Define the actual storage for the settings variables with their default values
        unsigned long long Mouse_Press_Down_Cooldown = 365;  // Milliseconds
        bool Word_Wrapping = true;
        bool ENABLE_GAMMA_CORRECTION = false;
        bool enableDRMBackend = true;
        
        namespace LOGGER {
            std::string File_Name = "";
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
        static void displayHelp(const std::vector<ArgumentDescriptor>& descriptors) {
            std::cout << "GGUI - Professional Command Line Interface\n";
            std::cout << "==========================================\n\n";
            std::cout << "Usage: GGUI [OPTIONS]\n\n";
            std::cout << "Available Options:\n";
            
            for (const auto& desc : descriptors) {
                std::cout << "  --" << desc.name;
                
                if (desc.requiresValue()) {
                    std::cout << "=<" << desc.getTypeName() << ">";
                }
                
                // Align descriptions
                std::string spacing(std::max(1, 30 - static_cast<int>(desc.name.length() + 
                    (desc.requiresValue() ? desc.getTypeName().length() + 3 : 0))), ' ');
                std::cout << spacing << desc.description << "\n";
            }
            
            std::cout << "\nExamples:\n";
            std::cout << "  GGUI --enableDRMBackend --mousePressCooldown=500\n";
            std::cout << "  GGUI -enableGammaCorrection --loggerFileName=\"debug.log\"\n";
            std::cout << "  GGUI enableWordWrapping mousePressCooldown=1000\n\n";
        }

        void parseCommandLineArguments(int argc, char** argv) {
            // Define argument descriptors with their handlers
            std::vector<ArgumentDescriptor> argumentDescriptors = {
                ArgumentDescriptor(
                    "mousePressCooldown",
                    ArgumentType::UNSIGNED_LONG,
                    "Set mouse press cooldown in milliseconds (default: 365)",
                    [](const std::string& value) {
                        try {
                            Mouse_Press_Down_Cooldown = std::stoull(value);
                        } catch (const std::exception& e) {
                            std::cerr << "Error: Invalid value for mousePressCooldown: " << value << std::endl;
                            std::cerr << "Expected an unsigned integer value." << std::endl;
                        }
                    }
                ),
                
                ArgumentDescriptor(
                    "enableWordWrapping",
                    ArgumentType::FLAG,
                    "Enable word wrapping (default: true)",
                    [](const std::string&) {
                        Word_Wrapping = true;
                    }
                ),
                
                ArgumentDescriptor(
                    "enableGammaCorrection",
                    ArgumentType::FLAG,
                    "Enable gamma correction (default: false)",
                    [](const std::string&) {
                        ENABLE_GAMMA_CORRECTION = true;
                    }
                ),
                
                ArgumentDescriptor(
                    "loggerFileName",
                    ArgumentType::STRING,
                    "Set logger file name (default: auto-generated)",
                    [](const std::string& value) {
                        std::string cleanValue = value;
                        // Remove quotes if present
                        if (cleanValue.length() >= 2 && cleanValue.front() == '"' && cleanValue.back() == '"') {
                            cleanValue = cleanValue.substr(1, cleanValue.length() - 2);
                        }
                        LOGGER::File_Name = cleanValue;
                    }
                ),
                
                ArgumentDescriptor(
                    "enableDRMBackend",
                    ArgumentType::FLAG,
                    "Enable DRM backend for hardware acceleration (default: false)",
                    [](const std::string&) {
                        enableDRMBackend = true;
                    }
                ),
                
                ArgumentDescriptor(
                    "help",
                    ArgumentType::FLAG,
                    "Display this help message and exit",
                    [&argumentDescriptors](const std::string&) {
                        displayHelp(argumentDescriptors);
                        exit(0);
                    }
                ),
                
                ArgumentDescriptor(
                    "h",
                    ArgumentType::FLAG,
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
                std::string argName, argValue;
                size_t equalPos = currentToken.find('=');
                
                if (equalPos != std::string::npos) {
                    argName = removeDashes(currentToken.substr(0, equalPos));
                    argValue = currentToken.substr(equalPos + 1);
                } else {
                    argName = removeDashes(currentToken);
                }

                // Find matching argument descriptor
                auto descriptorIt = std::find_if(argumentDescriptors.begin(), argumentDescriptors.end(),
                    [&argName](const ArgumentDescriptor& desc) {
                        return desc.name == argName;
                    });

                if (descriptorIt != argumentDescriptors.end()) {
                    // Found matching descriptor
                    if (descriptorIt->requiresValue()) {
                        if (equalPos != std::string::npos) {
                            // Value provided with = format
                            descriptorIt->handler(argValue);
                        } else if (i + 1 < tokens.size()) {
                            // Value should be in next token
                            std::string nextToken = tokens[++i];
                            // Don't consume if next token looks like another argument
                            if (!nextToken.empty() && nextToken[0] == '-' && nextToken.length() > 1) {
                                std::cerr << "Error: Argument --" << argName << " requires a value." << std::endl;
                                i--; // Back up so this token gets processed
                            } else {
                                descriptorIt->handler(nextToken);
                            }
                        } else {
                            std::cerr << "Error: Argument --" << argName << " requires a value." << std::endl;
                        }
                    } else {
                        // Flag argument
                        descriptorIt->handler("");
                    }
                } else {
                    // Unknown argument
                    if (!argName.empty()) {
                        std::cerr << "Warning: Unknown argument '" << argName << "'. Use --help for available options." << std::endl;
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
            if (LOGGER::File_Name.empty())
                LOGGER::File_Name = INTERNAL::constructLoggerFileName();
        }
    }
}