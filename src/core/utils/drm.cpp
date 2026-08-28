#include "drm.h"
#include "../core.h"

#include <cstdio>
#include <thread>

namespace GGUI {
    namespace DRM {
        const char* handshakePortLocation = "/tmp/GGDirect.gateway";
    
        // void packAbstractBuffer(char* destinationBuffer, std::vector<UTF>& abstractBuffer) {
        //     // cell* result = (cell*)destinationBuffer; // Static container

        //     for (unsigned int i = 0; i < abstractBuffer.size(); i++) {

        //         // cell currentCell = {{}, abstractBuffer[i].foreground, abstractBuffer[i].background};

        //         // // now we need to unpack the UTF compactString
        //         // if (abstractBuffer[i].size > sizeof(currentCell.utf)) {
        //         //     reportStack("UTF data: " + std::string(abstractBuffer[i].text) + " is too large for cell. Size: " + std::to_string(abstractBuffer[i].size) + ", max size: " + std::to_string(sizeof(currentCell.utf)));
        //         // } else {
        //         //     memcpy(currentCell.utf, abstractBuffer[i].text, abstractBuffer[i].size);
        //         // }

        //         // result[i] = currentCell;
        //     }
        // }

        void packet::input::translatePacketInputToGGUIInput(input::base* packetInput) {
            if (!packetInput) {
                return;
            }

            // Clean the keyboard states - save previous state
            core::inputManager->previousKeyboardState = core::inputManager->currentKeyboardState;

            // Update mouse position from packet
            if (packetInput->mouse.x >= 0 && packetInput->mouse.y >= 0) {
                currentMouse.position.x = packetInput->mouse.x;
                currentMouse.position.y = packetInput->mouse.y;
            }

            // Handle control key modifiers
            bool isPressed = (packetInput->modifiers & controlKey::PRESSED_DOWN) != controlKey::UNKNOWN;
            
            if ((packetInput->modifiers & controlKey::SHIFT) != controlKey::UNKNOWN) {
                core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::SHIFT] = converter::input::key(isPressed);
            }
            
            if ((packetInput->modifiers & controlKey::CTRL) != controlKey::UNKNOWN) {
                core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::CTRL] = converter::input::key(isPressed);
            }
            
            if ((packetInput->modifiers & controlKey::ALT) != controlKey::UNKNOWN) {
                core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ALT] = converter::input::key(isPressed);
            }
            
            if ((packetInput->modifiers & controlKey::SUPER) != controlKey::UNKNOWN) {
                core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::SUPER] = converter::input::key(isPressed);
            }
            
            if ((packetInput->modifiers & controlKey::ALTGR) != controlKey::UNKNOWN) {
                core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ALTGR] = converter::input::key(isPressed);
            }

            // Handle additional special keys
            switch (packetInput->additional) {
                case additionalKey::F1:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F1] = converter::input::key(isPressed);
                    break;
                case additionalKey::F2:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F2] = converter::input::key(isPressed);
                    break;
                case additionalKey::F3:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F3] = converter::input::key(isPressed);
                    break;
                case additionalKey::F4:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F4] = converter::input::key(isPressed);
                    break;
                case additionalKey::F5:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F5] = converter::input::key(isPressed);
                    break;
                case additionalKey::F6:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F6] = converter::input::key(isPressed);
                    break;
                case additionalKey::F7:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F7] = converter::input::key(isPressed);
                    break;
                case additionalKey::F8:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F8] = converter::input::key(isPressed);
                    break;
                case additionalKey::F9:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F9] = converter::input::key(isPressed);
                    break;
                case additionalKey::F10:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F10] = converter::input::key(isPressed);
                    break;
                case additionalKey::F11:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F11] = converter::input::key(isPressed);
                    break;
                case additionalKey::F12:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::F12] = converter::input::key(isPressed);
                    break;
                case additionalKey::ARROW_UP:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ARROW_UP] = converter::input::key(isPressed);
                    break;
                case additionalKey::ARROW_DOWN:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ARROW_DOWN] = converter::input::key(isPressed);
                    break;
                case additionalKey::ARROW_LEFT:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ARROW_LEFT] = converter::input::key(isPressed);
                    break;
                case additionalKey::ARROW_RIGHT:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ARROW_RIGHT] = converter::input::key(isPressed);
                    break;
                case additionalKey::HOME:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::HOME] = converter::input::key(isPressed);
                    break;
                case additionalKey::END:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::END] = converter::input::key(isPressed);
                    break;
                case additionalKey::PAGE_UP:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::PAGE_UP] = converter::input::key(isPressed);
                    break;
                case additionalKey::PAGE_DOWN:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::PAGE_DOWN] = converter::input::key(isPressed);
                    break;
                case additionalKey::INSERT:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::INSERT] = converter::input::key(isPressed);
                    break;
                case additionalKey::DEL:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::DELETE] = converter::input::key(isPressed);
                    break;
                case additionalKey::LEFT_CLICK:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::LEFT_CLICK] = converter::input::key(isPressed);
                    break;
                case additionalKey::MIDDLE_CLICK:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::MIDDLE_CLICK] = converter::input::key(isPressed);
                    break;
                case additionalKey::RIGHT_CLICK:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::RIGHT_CLICK] = converter::input::key(isPressed);
                    break;
                case additionalKey::SCROLL_UP:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::SCROLL_UP] = converter::input::key(isPressed);
                    break;
                case additionalKey::SCROLL_DOWN:
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::SCROLL_DOWN] = converter::input::key(isPressed);
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
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ENTER] = converter::input::key(isPressed);
                }
                else if (packetInput->key == '\t') {
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::TABULATOR] = converter::input::key(isPressed);
                    // handleTabulator();
                }
                else if (packetInput->key == '\b' || packetInput->key == 127) { // Backspace or DEL
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::DELETE] = converter::input::key(isPressed);
                }
                else if (packetInput->key == 27) { // ESC
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ESCAPE] = converter::input::key(isPressed);
                    // handleEscape();
                }
                else if (packetInput->key == ' ') { // Space
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::SPACE] = converter::input::key(isPressed);
                }
                else if (packetInput->key > (uint8_t)converter::input::key::types::SPACE && packetInput->key < (uint8_t)converter::input::key::types::DELETE) { // Printable ASCII characters
                    core::inputManager->currentKeyboardState[(uint8_t)packetInput->key] = converter::input::key(isPressed);

                    // Notify all loose constraints on letter event handlers.
                    core::inputManager->currentKeyboardState[(uint8_t)converter::input::key::types::ALL_LETTERS] = converter::input::key(isPressed);
                }
            }
        }

        #if _WIN32
        void connectDRMBackend() {}
        
        void sendBuffer(std::vector<UTF>&) {}

        void pollInputs() {}

        void translateInputs() {}

        void retryDRMConnect() {}
        #else
        
        // This will contain the open connection between DRM and this client.
        tcp::connection DRMConnection;

        void connectDRMBackend() {
            // First we'll read from the port holder file and read the port we'll need to connect to for the initial handshake.
            FILE* file = fopen(handshakePortLocation, "r");
            uint16_t DRMHandshakePort;
            if (file) {
                if (fscanf(file, "%hu", &DRMHandshakePort) != 1) {
                    GGUI::logger::log("Failed to read port from handshake file: " + std::string(handshakePortLocation));
                    fclose(file);
                    return;
                }
                fclose(file);
            } else {
                GGUI::logger::log("Could not locate handshake file: " + std::string(handshakePortLocation));
                return;
            }

            try {
                // Connect to the DRM backend's handshake port
                tcp::connection handshakeConnection = tcp::sender::getConnection(DRMHandshakePort);

                if (handshakeConnection.getHandle() < 0) {
                    GGUI::logger::log("DRM port: " + std::to_string(DRMHandshakePort) + " is not open.");
                    return;
                }

                // Create our own listener on any available port for the DRM backend to connect back to
                tcp::listener gguiListener(0); // 0 means system assigns an available port
                uint16_t gguiPort = gguiListener.getPort();

                // send our port to the DRM backend
                if (!handshakeConnection.Send(&gguiPort)) {
                    GGUI::logger::log("Failed to send GGUI port to DRM backend");
                    return;
                }

                // Wait for the DRM backend to connect back to us
                DRMConnection = gguiListener.Accept();

                // receive confirmation from the DRM backend
                uint16_t confirmationPort;
                if (!DRMConnection.receive(&confirmationPort)) {
                    GGUI::logger::log("Failed to receive confirmation from DRM backend");
                    return;
                }

                // Verify the confirmation port matches what we sent
                if (confirmationPort != gguiPort) {
                    GGUI::logger::log("Port confirmation mismatch. Expected: " + std::to_string(gguiPort) + ", got: " + std::to_string(confirmationPort));
                    return;
                }

                // The DRM will send straightaway the initial fullscreen dimensions for our client.
                char packetBuffer[packet::size];

                if (!DRMConnection.receive(packetBuffer, packet::size)) {
                    GGUI::logger::log("Failed to receive initial dimensions packet from DRM backend");
                    return;
                }

                packet::base* basePacket = reinterpret_cast<packet::base*>(packetBuffer);

                if (basePacket->packetType != packet::type::RESIZE) {
                    GGUI::logger::log("Expected initial dimensions packet, got: " + std::to_string(static_cast<int>(basePacket->packetType)));
                    return;
                }

                packet::resize::base* resizePacket = reinterpret_cast<packet::resize::base*>(packetBuffer);

                getRoot()->setDimensions(resizePacket->size.x, resizePacket->size.y);

            } catch (const std::exception& e) {
                GGUI::logger::log("DRM connection failed: " + std::string(e.what()));
                return;
            }
        }
        
        // void sendBuffer(std::vector<UTF>& abstractBuffer) {
        //     // Check if DRM connection is valid
        //     if (DRMConnection.getHandle() < 0) {
        //         GGUI::logger::log("DRM connection is not established");
        //         return;
        //     }

        //     size_t maximumBufferSize = getRoot()->getWidth() * getRoot()->getHeight() * sizeof(cell);

        //     static std::vector<char> packetBuffer = std::vector<char>();
            
        //     if (packetBuffer.size() != packet::size + maximumBufferSize)
        //         packetBuffer.resize(packet::size + maximumBufferSize);

        //     if (abstractBuffer.empty()) {
        //         packet::notify::base inform(packet::notify::type::EMPTY_BUFFER);
        //         // we write the inform into the packet buffer
        //         memcpy(packetBuffer.data(), &inform, sizeof(inform));
        //     }
        //     else {
        //         packet::base inform(packet::type::DRAW_BUFFER);
        //         // we write the inform into the packet buffer
        //         memcpy(packetBuffer.data(), &inform, sizeof(inform));

        //         // Now we need to pack the abstract buffer into a vector of cells
        //         packAbstractBuffer(packetBuffer.data() + packet::size, abstractBuffer);
        //     }
            
        //     if (!DRMConnection.Send(packetBuffer.data(), packet::size + maximumBufferSize)){    // Tell DRM to expect an draw buffer
        //         GGUI::logger::log("Failed to send draw buffer header");

        //         std::this_thread::sleep_for(std::chrono::milliseconds(TIME::SECOND)); // Wait for the cleanup of tcp packages in the DRM backend
        //     }

        //     std::this_thread::sleep_for(std::chrono::milliseconds(tcp::pollingRate));
        // }

        void retryDRMConnect() {
            while (DRMConnection.getHandle() < 0) {
                // Attempt to reconnect
                GGUI::logger::log("Retrying DRM connection...");

                connectDRMBackend();

                if (DRMConnection.getHandle() < 0) {
                    GGUI::logger::log("DRM connection failed, retrying in " + std::to_string(failRetryWaitTime.count()) + " seconds...");
                    std::this_thread::sleep_for(failRetryWaitTime);
                } else {
                    GGUI::logger::log("DRM connection established successfully");
                }
            }
        }

        void close() {
            // send INFORM::CLOSED to the DRM backend
            char packetBuffer[packet::size];

            // Check if DRM connection is valid before attempting to close
            if (DRMConnection.getHandle() < 0) {
                GGUI::logger::log("DRM connection is not established, cannot send close notification");
                return;
            }

            // Create a notification packet to inform DRM backend that we're closing
            packet::notify::base inform(packet::notify::type::CLOSED);
            
            // Copy the packet to the buffer
            memcpy(packetBuffer, &inform, sizeof(inform));

            // send the close notification to the DRM backend
            if (!DRMConnection.Send(packetBuffer, packet::size)) {
                GGUI::logger::log("Failed to send close notification to DRM backend");
            }

            // Close the connection
            DRMConnection.close();
        }

        char rawPacketBuffer[packet::size];              
        void pollInputs() {
            // We wait here until the DRM connection has been established:
            while (DRMConnection.getHandle() < 0) {
                std::this_thread::sleep_for(failRetryWaitTime);
            }

            // First we will wait for incoming packets
            if (!DRMConnection.receive(rawPacketBuffer, packet::size)) {
                GGUI::logger::log("Failed to receive packet from DRM backend");
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

                    getRoot()->setDimensions(resizePacket->size.x, resizePacket->size.y);
                    break;
                }
            case packet::type::NOTIFY:
                {
                    // Now we can simply cast to the notify packet and read the type
                    packet::notify::base* notifyPacket = reinterpret_cast<packet::notify::base*>(rawPacketBuffer);

                    if (notifyPacket->notifyType == packet::notify::type::CLOSED) {
                        // This means we got an termination request, so we need to shutdown gracefully.
                        GGUI::logger::log("Received close notification from DRM backend, shutting down...");

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
                GGUI::logger::log("Received unexpected DRAW_BUFFER packet from DRM backend");
                break;
            case packet::type::UNKNOWN:
            default:
                GGUI::logger::log("Received unknown packet type from DRM backend: " + std::to_string(static_cast<int>(basePacket->packetType)));
                break;
            }
        }

        #endif
    
    }
}