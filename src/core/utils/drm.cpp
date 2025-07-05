#include "drm.h"
#include <cstdio>

namespace GGUI {
    namespace INTERNAL {
        namespace DRM {
            const char* handshakePortLocation = "/tmp/GGDirect.gateway";
        
            std::vector<cell>* packAbstractBuffer(std::vector<UTF>& abstractBuffer) {
                std::vector<cell>* result = new std::vector<cell>();
                result->resize(abstractBuffer.size());

                for (int i = 0; i < abstractBuffer.size(); i++) {

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
            void connectDRMBackend() {}  // Currently DRM is only supported on linux side of GGUI
            void sendBuffer(std::vector<UTF>* abstractBuffer, unsigned int width, unsigned int height) {}
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
                    GGUI::INTERNAL::LOGGER::Log("Failed to open handshake file for reading");
                    return;
                }

                try {
                    // Connect to the DRM backend's handshake port
                    tcp::connection handshakeConnection = tcp::sender::getConnection(DRMHandshakePort);

                    // Create our own listener on any available port for the DRM backend to connect back to
                    tcp::listener gguiListener(0); // 0 means system assigns an available port
                    uint16_t gguiPort = gguiListener.getPort();

                    // Send our port to the DRM backend
                    if (!handshakeConnection.Send(&gguiPort, 1)) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to send GGUI port to DRM backend");
                        return;
                    }

                    // Wait for the DRM backend to connect back to us
                    DRMConnection = gguiListener.Accept();

                    // Receive confirmation from the DRM backend
                    uint16_t confirmationPort;
                    if (!DRMConnection.Receive(&confirmationPort, 1)) {
                        GGUI::INTERNAL::LOGGER::Log("Failed to receive confirmation from DRM backend");
                        return;
                    }

                    // Verify the confirmation port matches what we sent
                    if (confirmationPort != gguiPort) {
                        GGUI::INTERNAL::LOGGER::Log("Port confirmation mismatch from DRM backend");
                        return;
                    }
                } catch (const std::exception& e) {
                    GGUI::INTERNAL::LOGGER::Log("DRM connection failed: " + std::string(e.what()));
                    return;
                }
            }
            
            void sendBuffer(std::vector<UTF>& abstractBuffer, unsigned int width, unsigned int height) {
                // Validate input parameters
                if (abstractBuffer.empty()) {
                    GGUI::INTERNAL::LOGGER::Log("Cannot send null buffer to DRM backend");
                    return;
                }

                // Check if DRM connection is valid
                if (DRMConnection.getHandle() < 0) {
                    GGUI::INTERNAL::LOGGER::Log("DRM connection is not established");
                    return;
                }

                // Pack dimensions into a structure
                struct {
                    int x;
                    int y;
                } dimensions = {width, height};

                // First send the dimensions of this buffer (send 1 structure)
                if (!DRMConnection.Send(&dimensions, 1)) {
                    GGUI::INTERNAL::LOGGER::Log("Failed to send dimensions to DRM backend");
                    return;
                }

                // Now we need to pack the abstract buffer into a vector of cells
                std::vector<cell>* packedBuffer = packAbstractBuffer(abstractBuffer);

                // Now send the actual buffer data (send abstractBuffer->size() UTF elements)
                if (!packedBuffer->empty() && !DRMConnection.Send(packedBuffer->data(), packedBuffer->size())) {
                    GGUI::INTERNAL::LOGGER::Log("Failed to send buffer data to DRM backend");
                    return;
                }
            }

            void sendEmptyBuffer() {
                // Pack dimensions into a structure
                struct {
                    unsigned int x;
                    unsigned int y;
                } dimensions = {0, 0};

                // Send the dimensions of this empty buffer (send 1 structure)
                if (!DRMConnection.Send(&dimensions, 1)) {
                    GGUI::INTERNAL::LOGGER::Log("Failed to send dimensions of empty buffer to DRM backend");
                    return;
                }
            }

            #endif
        
        }
    }
}