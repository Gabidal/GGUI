#include "drm.h"
#include "../renderer.h"

#include <cstdio>
#include <thread>
#include <chrono>

namespace GGUI {
    namespace INTERNAL {
        
        // Forward declarations for functions used from renderer.cpp
        extern void handleEscape();
        extern void handleTabulator();
        extern std::unordered_map<std::string, buttonState> PREVIOUS_KEYBOARD_STATES;
        extern std::unordered_map<std::string, buttonState> KEYBOARD_STATES;
        extern std::vector<input*> Inputs;
        extern IVector3 Mouse;
        
        namespace DRM {
            const char* handshakePortLocation = "/tmp/GGDirect.gateway";
        
            void packAbstractBuffer(char* destinationBuffer, std::vector<UTF>& abstractBuffer) {
                cell* result = (cell*)destinationBuffer; // Static container

                for (unsigned int i = 0; i < abstractBuffer.size(); i++) {

                    cell currentCell = {{}, abstractBuffer[i].foreground, abstractBuffer[i].background};

                    // now we need to unpack the UTF compactString
                    if (abstractBuffer[i].is(COMPACT_STRING_FLAG::IS_ASCII))
                        currentCell.utf[0] = abstractBuffer[i].getAscii();
                    else {
                        if (abstractBuffer[i].size > sizeof(currentCell.utf)) {
                            reportStack("UTF data: " + std::string(abstractBuffer[i].getUnicode()) + " is too large for cell. Size: " + std::to_string(abstractBuffer[i].size) + ", max size: " + std::to_string(sizeof(currentCell.utf)));
                        } else {
                            memcpy(currentCell.utf, abstractBuffer[i].getUnicode(), abstractBuffer[i].size);
                        }

                    }

                    result[i] = currentCell;
                }
            }

            void packet::input::translatePacketInputToGGUIInput(input::base* packetInput) {
                if (!packetInput) {
                    return;
                }

                // Clean the keyboard states - save previous state
                INTERNAL::PREVIOUS_KEYBOARD_STATES = INTERNAL::KEYBOARD_STATES;

                // Update mouse position from packet
                if (packetInput->mouse.X >= 0 && packetInput->mouse.Y >= 0) {
                    INTERNAL::Mouse.X = packetInput->mouse.X;
                    INTERNAL::Mouse.Y = packetInput->mouse.Y;
                }

                // Handle control key modifiers
                bool isPressed = (packetInput->modifiers & controlKey::PRESSED_DOWN) != controlKey::UNKNOWN;
                
                if ((packetInput->modifiers & controlKey::SHIFT) != controlKey::UNKNOWN) {
                    INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::SHIFT));
                    INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SHIFT] = INTERNAL::buttonState(isPressed);
                }
                
                if ((packetInput->modifiers & controlKey::CTRL) != controlKey::UNKNOWN) {
                    INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::CONTROL));
                    INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = INTERNAL::buttonState(isPressed);
                }
                
                if ((packetInput->modifiers & controlKey::ALT) != controlKey::UNKNOWN) {
                    INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::ALT));
                    INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ALT] = INTERNAL::buttonState(isPressed);
                }
                
                if ((packetInput->modifiers & controlKey::SUPER) != controlKey::UNKNOWN) {
                    INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::SUPER));
                    INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SUPER] = INTERNAL::buttonState(isPressed);
                }
                
                if ((packetInput->modifiers & controlKey::ALTGR) != controlKey::UNKNOWN) {
                    // Note: ALTGR is handled as ALT in GGUI
                    INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::ALT));
                    INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ALT] = INTERNAL::buttonState(isPressed);
                }

                // Handle additional special keys
                switch (packetInput->additional) {
                    case additionalKey::F1:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F1));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F1] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F2:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F2));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F2] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F3:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F3));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F3] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F4:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F4));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F4] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F5:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F5));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F5] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F6:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F6));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F6] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F7:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F7));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F7] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F8:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F8));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F8] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F9:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F9));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F9] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F10:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F10));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F10] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F11:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F11));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F11] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::F12:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::F12));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::F12] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::ARROW_UP:
                        INTERNAL::Inputs.push_back(new GGUI::input(0, GGUI::constants::UP));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::UP] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::ARROW_DOWN:
                        INTERNAL::Inputs.push_back(new GGUI::input(0, GGUI::constants::DOWN));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::DOWN] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::ARROW_LEFT:
                        INTERNAL::Inputs.push_back(new GGUI::input(0, GGUI::constants::LEFT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::LEFT] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::ARROW_RIGHT:
                        INTERNAL::Inputs.push_back(new GGUI::input(0, GGUI::constants::RIGHT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::RIGHT] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::HOME:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::HOME));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::HOME] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::END:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::END));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::END] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::PAGE_UP:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::PAGE_UP));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::PAGE_UP] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::PAGE_DOWN:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::PAGE_DOWN));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::PAGE_DOWN] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::INSERT:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::INSERT));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::INSERT] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::DELETE:
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::DELETE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::DELETE] = INTERNAL::buttonState(isPressed);
                        break;
                    case additionalKey::LEFT_CLICK:
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT] = INTERNAL::buttonState(isPressed);
                        if (isPressed) {
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_LEFT].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        break;
                    case additionalKey::MIDDLE_CLICK:
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE] = INTERNAL::buttonState(isPressed);
                        if (isPressed) {
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_MIDDLE].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        break;
                    case additionalKey::RIGHT_CLICK:
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT] = INTERNAL::buttonState(isPressed);
                        if (isPressed) {
                            INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_RIGHT].Capture_Time = std::chrono::high_resolution_clock::now();
                        }
                        break;
                    case additionalKey::SCROLL_UP:
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP] = INTERNAL::buttonState(true);
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN] = INTERNAL::buttonState(false);
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP].Capture_Time = std::chrono::high_resolution_clock::now();
                        break;
                    case additionalKey::SCROLL_DOWN:
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN] = INTERNAL::buttonState(true);
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_UP] = INTERNAL::buttonState(false);
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::MOUSE_SCROLL_DOWN].Capture_Time = std::chrono::high_resolution_clock::now();
                        break;
                    case additionalKey::UNKNOWN:
                    default:
                        // Handle regular key input
                        break;
                }

                // Handle regular character key input
                if (packetInput->key != 0) {
                    // Check for special characters
                    if (packetInput->key == '\n' || packetInput->key == '\r') {
                        INTERNAL::Inputs.push_back(new GGUI::input('\n', GGUI::constants::ENTER));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ENTER] = INTERNAL::buttonState(isPressed);
                    }
                    else if (packetInput->key == '\t') {
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::TAB));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::TAB] = INTERNAL::buttonState(isPressed);
                        handleTabulator();
                    }
                    else if (packetInput->key == '\b' || packetInput->key == 127) { // Backspace or DEL
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::BACKSPACE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::BACKSPACE] = INTERNAL::buttonState(isPressed);
                    }
                    else if (packetInput->key == 27) { // ESC
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::ESCAPE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::ESC] = INTERNAL::buttonState(isPressed);
                        handleEscape();
                    }
                    else if (packetInput->key == ' ') { // Space
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::SPACE));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::SPACE] = INTERNAL::buttonState(isPressed);
                    }
                    else if (packetInput->key >= 32 && packetInput->key <= 126) { // Printable ASCII characters
                        // Only process key press events for regular characters
                        if (isPressed) {
                            INTERNAL::Inputs.push_back(new GGUI::input(packetInput->key, GGUI::constants::KEY_PRESS));
                        }
                    }
                    else if (packetInput->key >= 1 && packetInput->key <= 26) { // Ctrl+A to Ctrl+Z
                        // Convert back to the corresponding letter
                        char ctrlChar = packetInput->key + 'A' - 1;
                        INTERNAL::Inputs.push_back(new GGUI::input(' ', GGUI::constants::CONTROL));
                        INTERNAL::KEYBOARD_STATES[KEYBOARD_BUTTONS::CONTROL] = INTERNAL::buttonState(isPressed);
                        
                        // Also add the character if it's a key press
                        if (isPressed) {
                            INTERNAL::Inputs.push_back(new GGUI::input(ctrlChar, GGUI::constants::KEY_PRESS));
                        }
                    }
                }
            }

            #if _WIN32
            void connectDRMBackend() {}
            
            void sendBuffer(std::vector<UTF>& abstractBuffer) {}
            #else
            
            // This will contain the open connection between DRM and this client.
            tcp::connection DRMConnection;

            void connectDRMBackend() {
                // First we'll read from the port holder file and read the port we'll need to connect to for the initial handshake.
                FILE* file = fopen(handshakePortLocation, "r");
                uint16_t DRMHandshakePort;
                if (file) {
                    if (fscanf(file, "%hu", &DRMHandshakePort) != 1) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to read port from handshake file: " + std::string(handshakePortLocation));
                        fclose(file);
                        return;
                    }
                    fclose(file);
                } else {
                    GGUI::INTERNAL::LOGGER::Log("Could not locate handshake file: " + std::string(handshakePortLocation));
                    return;
                }

                try {
                    // Connect to the DRM backend's handshake port
                    tcp::connection handshakeConnection = tcp::sender::getConnection(DRMHandshakePort);

                    if (handshakeConnection.getHandle() < 0) {
                        GGUI::INTERNAL::LOGGER::Log("DRM port: " + std::to_string(DRMHandshakePort) + " is not open.");
                        return;
                    }

                    // Create our own listener on any available port for the DRM backend to connect back to
                    tcp::listener gguiListener(0); // 0 means system assigns an available port
                    uint16_t gguiPort = gguiListener.getPort();

                    // Send our port to the DRM backend
                    if (!handshakeConnection.Send(&gguiPort)) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to send GGUI port to DRM backend");
                        return;
                    }

                    // Wait for the DRM backend to connect back to us
                    DRMConnection = gguiListener.Accept();

                    // Receive confirmation from the DRM backend
                    uint16_t confirmationPort;
                    if (!DRMConnection.Receive(&confirmationPort)) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to receive confirmation from DRM backend");
                        return;
                    }

                    // Verify the confirmation port matches what we sent
                    if (confirmationPort != gguiPort) {
                        GGUI::INTERNAL::LOGGER::Log("Port confirmation mismatch. Expected: " + std::to_string(gguiPort) + ", got: " + std::to_string(confirmationPort));
                        return;
                    }

                    // The DRM will send straightaway the initial fullscreen dimensions for our client.
                    char packetBuffer[packet::size];

                    if (!DRMConnection.Receive(packetBuffer, packet::size)) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to receive initial dimensions packet from DRM backend");
                        return;
                    }

                    packet::base* basePacket = reinterpret_cast<packet::base*>(packetBuffer);

                    if (basePacket->packetType != packet::type::RESIZE) {
                        GGUI::INTERNAL::LOGGER::Log("Expected initial dimensions packet, got: " + std::to_string(static_cast<int>(basePacket->packetType)));
                        return;
                    }

                    packet::resize::base* resizePacket = reinterpret_cast<packet::resize::base*>(packetBuffer);

                    Main->setDimensions(resizePacket->size.X, resizePacket->size.Y);

                } catch (const std::exception& e) {
                    GGUI::INTERNAL::LOGGER::Log("DRM connection failed: " + std::string(e.what()));
                    return;
                }
            }
            
            void sendBuffer(std::vector<UTF>& abstractBuffer) {
                // Check if DRM connection is valid
                if (DRMConnection.getHandle() < 0) {
                    GGUI::INTERNAL::LOGGER::Log("DRM connection is not established");
                    return;
                }

                size_t maximumBufferSize = INTERNAL::Main->getWidth() * INTERNAL::Main->getHeight() * sizeof(cell);

                static std::vector<char> packetBuffer = std::vector<char>();
                
                if (packetBuffer.size() != packet::size + maximumBufferSize)
                    packetBuffer.resize(packet::size + maximumBufferSize);

                if (abstractBuffer.empty()) {
                    packet::notify::base inform(packet::notify::type::EMPTY_BUFFER);
                    // we write the inform into the packet buffer
                    memcpy(packetBuffer.data(), &inform, sizeof(inform));
                }
                else {
                    packet::base inform(packet::type::DRAW_BUFFER);
                    // we write the inform into the packet buffer
                    memcpy(packetBuffer.data(), &inform, sizeof(inform));

                    // Now we need to pack the abstract buffer into a vector of cells
                    packAbstractBuffer(packetBuffer.data() + packet::size, abstractBuffer);
                }
                
                if (!DRMConnection.Send(packetBuffer.data(), packet::size + maximumBufferSize)){    // Tell DRM to expect an draw buffer
                    GGUI::INTERNAL::LOGGER::Log("Failed to send draw buffer header");
                }
            }

            void retryDRMConnect() {
                while (DRMConnection.getHandle() < 0) {
                    // Attempt to reconnect
                    GGUI::INTERNAL::LOGGER::Log("Retrying DRM connection...");

                    connectDRMBackend();

                    if (DRMConnection.getHandle() < 0) {
                        GGUI::INTERNAL::LOGGER::Log("DRM connection failed, retrying in " + std::to_string(failRetryWaitTime/TIME::SECOND) + " seconds...");
                        std::this_thread::sleep_for(std::chrono::milliseconds(failRetryWaitTime));
                    } else {
                        GGUI::INTERNAL::LOGGER::Log("DRM connection established successfully");
                    }
                }
            }

            void close() {
                // Send INFORM::CLOSED to the DRM backend
                char packetBuffer[packet::size];

                // Check if DRM connection is valid before attempting to close
                if (DRMConnection.getHandle() < 0) {
                    GGUI::INTERNAL::LOGGER::Log("DRM connection is not established, cannot send close notification");
                    return;
                }

                // Create a notification packet to inform DRM backend that we're closing
                packet::notify::base inform(packet::notify::type::CLOSED);
                
                // Copy the packet to the buffer
                memcpy(packetBuffer, &inform, sizeof(inform));

                // Send the close notification to the DRM backend
                if (!DRMConnection.Send(packetBuffer, packet::size)) {
                    GGUI::INTERNAL::LOGGER::Log("Failed to send close notification to DRM backend");
                }

                // Close the connection
                DRMConnection.close();
            }

            char rawPacketBuffer[packet::size];              
            void pollInputs() {
                // We wait here until the DRM connection has been established:
                while (DRMConnection.getHandle() < 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(failRetryWaitTime));
                }

                // First we will wait for incoming packets
                if (!DRMConnection.Receive(rawPacketBuffer, packet::size)) {
                    GGUI::INTERNAL::LOGGER::Log("Failed to receive packet from DRM backend");
                    return; // Exit if we can't receive data
                }
            }

            void translateInputs() {
                // Now we can cast it into the base packet type and check the type
                packet::base* basePacket = reinterpret_cast<packet::base*>(rawPacketBuffer);

                switch (basePacket->packetType)
                {
                case packet::type::RESIZE:
                    {
                        // Now we can simply cast to the resize packet and read the new size and assign it to Main element
                        packet::resize::base* resizePacket = reinterpret_cast<packet::resize::base*>(rawPacketBuffer);
    
                        Main->setDimensions(resizePacket->size.X, resizePacket->size.Y);
                        break;
                    }
                case packet::type::NOTIFY:
                    {
                        // Now we can simply cast to the notify packet and read the type
                        packet::notify::base* notifyPacket = reinterpret_cast<packet::notify::base*>(rawPacketBuffer);

                        if (notifyPacket->notifyType == packet::notify::type::CLOSED) {
                            // This means we got an termination request, so we need to shutdown gracefully.
                            GGUI::INTERNAL::LOGGER::Log("Received close notification from DRM backend, shutting down...");

                            EXIT(0); // Exit the application gracefully
                        }
                        break;
                    }
                case packet::type::INPUT:
                    {
                        // Now we can simply cast to the input packet and read the input data
                        packet::input::base* inputPacket = reinterpret_cast<packet::input::base*>(rawPacketBuffer);

                        // Translate the DRM input packet to GGUI input format
                        packet::input::translatePacketInputToGGUIInput(inputPacket);
                        break;
                    }
                case packet::type::DRAW_BUFFER:
                    // DRM backend should not send draw buffer packets to us, ignore
                    GGUI::INTERNAL::LOGGER::Log("Received unexpected DRAW_BUFFER packet from DRM backend");
                    break;
                case packet::type::UNKNOWN:
                default:
                    GGUI::INTERNAL::LOGGER::Log("Received unknown packet type from DRM backend: " + std::to_string(static_cast<int>(basePacket->packetType)));
                    break;
                }
            }

            #endif
        
        }
    }
}