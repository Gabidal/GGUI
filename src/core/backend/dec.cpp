#include "dec.h"
#include "terminal.h"

namespace GGUI {
    namespace terminal {
        namespace dec {
            
            namespace VT100 {
                ecma::configuration::cellPatch csiPatch(ecma::table::C1::CSI);

                ecma::configuration::page G1(ecma::configuration::layout::graphical::getRelativeGraphicalPageLayout(ecma::configuration::layout::graphical::type::B));
                ecma::configuration::page G3(ecma::configuration::layout::graphical::getRelativeGraphicalPageLayout(ecma::configuration::layout::graphical::type::B));
            
                namespace sequences {
                    
                    namespace cursorControlFunctions {
                        void operate_CURSOR_UP(ecma::sequence::base* /*ignored*/) {
                            currentStates->ecmaComponents.activePresentationPosition.y--;
                        }

                        void operate_CURSOR_DOWN(ecma::sequence::base* /*ignored*/) {
                            currentStates->ecmaComponents.activePresentationPosition.y++;
                        }

                        void operate_CURSOR_LEFT(ecma::sequence::base* /*ignored*/) {
                            currentStates->ecmaComponents.activePresentationPosition.x--;
                        }

                        void operate_CURSOR_RIGHT(ecma::sequence::base* /*ignored*/) {
                            currentStates->ecmaComponents.activePresentationPosition.x++;
                        }
                    }

                    namespace miscellaneousControlFunctions {
                        void operate_DEVICE_ATTRIBUTES(ecma::sequence::base* input) {
                            auto controlSequence = miscellaneousControlFunctions::DEVICE_ATTRIBUTES.transform(input);

                            auto params = controlSequence.getParameters();

                            if (params.size() == 1) {   // This is the request for identification, which is already implemented in the ecma.cpp, so we can just reroute into there.
                                ecma::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(input);
                            } else if (params.size() == 2 && params.front().hasSecondaries()) {
                                const auto& [questionMark, modelNumber] = params.front().getPrimaryValueAndSecondaries().front();

                                if (
                                    (ecma::table::parameters)questionMark == ecma::table::parameters::PRIVATE &&
                                    (uint8_t)modelNumber == 1
                                ) {
                                    currentStates->decComponents.VT100Components.activeDeviceAttributes = (deviceAttributeResponseTypes)params.back().getValueAsInteger();
                                    currentStates->decComponents.VT100Components.enabled = true;
                                }
                            } else {
                                assert(false);
                            }
                        }

                        void operate_CONFIDENCE_TEST(ecma::sequence::base*) {
                            TODO("Not finished!");
                        }
                    }

                    namespace modeSettingFunctions {
                        void operate_RESET_MODE(ecma::sequence::base* input) {
                            auto controlSequence = modeSettingFunctions::RESET_MODE.transform(input);

                            auto params = controlSequence.getParameters();

                            assert(params.size() > 0);

                            // We can now check if the first parameter is a '?' or not, if not, then we proceed to call the base ecma implementation of this function
                            if ((uint8_t)params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE) {
                                for (size_t i = 1; i < params.size(); i++) {
                                    auto typed = params[i].getValueAsInteger();

                                    if (GGUI::table::contains<modeTypes>(typed)) {
                                        currentStates->decComponents.VT100Components.modes.set(typed, static_cast<bool>(ecma::mode::values::RESET));
                                    }

                                }
                            } else {
                                ecma::sequences::modeSettingFunctions::operate_RESET_MODE(input);
                            }
                        }

                        void operate_SET_MODE(ecma::sequence::base* input) {
                            auto controlSequence = modeSettingFunctions::SET_MODE.transform(input);

                            auto params = controlSequence.getParameters();

                            assert(params.size() > 0);

                            // We can now check if the first parameter is a '?' or not, if not, then we proceed to call the base ecma implementation of this function
                            if ((uint8_t)params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE) {
                                for (size_t i = 1; i < params.size(); i++) {
                                    auto typed = params[i].getValueAsInteger();

                                    if (GGUI::table::contains<modeTypes>(typed)) {
                                        currentStates->decComponents.VT100Components.modes.set(typed, static_cast<bool>(ecma::mode::values::SET));
                                    }

                                }
                            } else {
                                ecma::sequences::modeSettingFunctions::operate_SET_MODE(input);
                            }
                        }
                    }
                }
            }

            namespace VT220 {
                ecma::configuration::cellPatch csiPatch(ecma::table::C1::CSI);

                namespace sequences {
                    // Override of ecma miscellaneousControlFunctions
                    namespace miscellaneousControlFunctions {
                        void operate_DEVICE_ATTRIBUTES(ecma::sequence::base* input) {
                            auto controlSequence = miscellaneousControlFunctions::DEVICE_ATTRIBUTES.transform(input);

                            auto params = controlSequence.getParameters();

                            if (!params.empty()) {
                                // VT220 and VT320 both share same device feature list
                                if (
                                    params.front().hasSecondaries() && 
                                    (uint8_t)params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE && 
                                    (
                                        (uint8_t)params.front().getPrimaryValueAndSecondaries().back().first == VT220::deviceAttributeResponseID || 
                                        (uint8_t)params.front().getPrimaryValueAndSecondaries().back().first == VT320::deviceAttributeResponseID
                                    )
                                ) {
                                    currentStates->decComponents.VT220Components.activeDeviceAttributes.resize(params.size());

                                    for (size_t i = 1; i < params.size(); i++) {    // i=1, to skip the deviceAttributeResponseID
                                        currentStates->decComponents.VT220Components.activeDeviceAttributes[i] = (deviceAttributeResponseTable)params[i].getValueAsInteger();
                                    }

                                    currentStates->decComponents.VT220Components.enabled = true;
                                } else {    // pass onto next lower level
                                    VT100::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(input);
                                }
                            }
                        }
                    }
                }
            }

            namespace VT420 {
                ecma::configuration::cellPatch csiPatch(ecma::table::C1::CSI);

                namespace sequences {
                    // Override of ecma miscellaneousControlFunctions
                    namespace miscellaneousControlFunctions {
                        void operate_DEVICE_ATTRIBUTES(ecma::sequence::base* input) {
                            auto controlSequence = miscellaneousControlFunctions::DEVICE_ATTRIBUTES.transform(input);

                            auto params = controlSequence.getParameters();

                            if (!params.empty()) {
                                if (
                                    params.front().hasSecondaries() &&
                                    (uint8_t)params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE && (
                                        (uint8_t)params.front().getPrimaryValueAndSecondaries().back().first == VT420::deviceAttributeResponseID ||
                                        (uint8_t)params.front().getPrimaryValueAndSecondaries().back().first == VT420::emulatedVT100DeviceAttributeResponseID
                                    )
                                ) {   // VT420 and VT510 both share same ID and feature list.
                                    currentStates->decComponents.VT420Components.activeDeviceAttributes.resize(params.size());

                                    for (size_t i = 1; i < params.size(); i++) {    // i=1, to skip the deviceAttributeResponseID
                                        currentStates->decComponents.VT420Components.activeDeviceAttributes[i] = (deviceAttributeResponseTable)params[i].getValueAsInteger();
                                    }

                                    // If the responder is an emulator pretending to be a VT100, we can enable VT100 module, but keep it with NO_OPTIONS
                                    if ((uint8_t)params.front().getPrimaryValueAndSecondaries().back().first == VT420::emulatedVT100DeviceAttributeResponseID) {
                                        currentStates->decComponents.VT100Components.activeDeviceAttributes = VT100::deviceAttributeResponseTypes::NO_OPTIONS;
                                        currentStates->decComponents.VT100Components.enabled = true;
                                    }

                                    currentStates->decComponents.VT420Components.enabled = true;
                                } else {    // pass onto next lower level
                                    VT220::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(input);
                                }
                            }
                        }
                    }
                }
            }

            bool components::verifyExtensions(ecma::sequence::base* parsed) {
                if (
                    parsed->getType() == ecma::sequence::types::CSI && 
                    (ecma::table::finalWithoutIntermediate)parsed->getPostfix().getFinalByte() == ecma::table::finalWithoutIntermediate::DA
                ) {
                    VT420::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(parsed);
                }

                return isEnabled();
            }
        }
    }
}