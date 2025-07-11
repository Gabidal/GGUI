#include "drm.h"
#include "../renderer.h"

#include <cstdio>
#include <thread>
#include <chrono>

namespace GGUI {
    namespace INTERNAL {
        namespace DRM {
            const char* handshakePortLocation = "/tmp/GGDirect.gateway";
        
            std::vector<cell>* packAbstractBuffer(std::vector<UTF>& abstractBuffer) {
                std::vector<cell>* result = new std::vector<cell>();
                result->resize(abstractBuffer.size());

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

                    (*result)[i] = currentCell;
                }

                return result;
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
                    fscanf(file, "%hu", &DRMHandshakePort);
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

                char* packetBuffer = new char[packet::size + maximumBufferSize];

                if (abstractBuffer.empty()) {
                    packet::notify::base inform(packet::notify::type::EMPTY_BUFFER);
                    // we write the inform into the packet buffer
                    memcpy(packetBuffer, &inform, sizeof(inform));
                }
                else {
                    packet::base inform(packet::type::DRAW_BUFFER);
                    // we write the inform into the packet buffer
                    memcpy(packetBuffer, &inform, sizeof(inform));

                    // Now we need to pack the abstract buffer into a vector of cells
                    std::vector<cell>* packedBuffer = packAbstractBuffer(abstractBuffer);
                    
                    if (packedBuffer->empty()) {
                        GGUI::INTERNAL::LOGGER::Log("Buffer packing failed");
                        delete[] packetBuffer;
                        return;
                    }
                    
                    // Now we write the packedBuffer into the packetBuffer
                    memcpy(packetBuffer + packet::size, packedBuffer->data(), packedBuffer->size() * sizeof(cell));
                }
                
                if (!DRMConnection.Send(packetBuffer, packet::size + maximumBufferSize)){    // Tell DRM to expect an draw buffer
                    GGUI::INTERNAL::LOGGER::Log("Failed to send draw buffer header");
                }

                delete[] packetBuffer;
            }

            void retryDRMConnect() {
                while (DRMConnection.getHandle() < 0) {
                    // Attempt to reconnect
                    GGUI::INTERNAL::LOGGER::Log("Retrying DRM connection...");

                    connectDRMBackend();

                    if (DRMConnection.getHandle() < 0) {
                        GGUI::INTERNAL::LOGGER::Log("DRM connection failed, retrying in 5 seconds...");
                        std::this_thread::sleep_for(std::chrono::seconds(5));
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

            #endif
        
        }
    }
}