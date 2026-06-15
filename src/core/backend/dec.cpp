#include "dec.h"
#include "terminal.h"

namespace GGUI {
    namespace terminal {
        namespace dec {
            
            namespace VT100 {
                ecma::table::configuration::page G1(ecma::table::configuration::layout::graphical::getRelativeGraphicalPageLayout(ecma::table::configuration::layout::graphical::type::B));
                ecma::table::configuration::page G3(ecma::table::configuration::layout::graphical::getRelativeGraphicalPageLayout(ecma::table::configuration::layout::graphical::type::B));
            
                namespace sequences {
                    
                    namespace cursorControlFunctions {
                        void operate_CURSOR_UP(ecma::sequence::base* /*ignored*/) {
                            currentStates.ecmaComponents.activePresentationPosition.y--;
                        }

                        void operate_CURSOR_DOWN(ecma::sequence::base* /*ignored*/) {
                            currentStates.ecmaComponents.activePresentationPosition.y++;
                        }

                        void operate_CURSOR_LEFT(ecma::sequence::base* /*ignored*/) {
                            currentStates.ecmaComponents.activePresentationPosition.x--;
                        }

                        void operate_CURSOR_RIGHT(ecma::sequence::base* /*ignored*/) {
                            currentStates.ecmaComponents.activePresentationPosition.x++;
                        }
                    }

                    namespace miscellaneousControlFunctions {
                        void operate_DEVICE_ATTRIBUTES(ecma::sequence::base* input) {
                            auto controlSequence = static_cast<ecma::sequence::control<ecma::sequence::parameter::selectable<deviceAttributeResponseTypes>>*>(input);

                            auto params = controlSequence->getParameters();

                            if (params.size() == 1) {   // This is the request for identification, which is already implemented in the ecma.cpp, so we can just reroute into there.
                                ecma::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(input);
                            } else if (params.size() == 2) {
                                currentStates.decComponents.VT100Components.activeDeviceAttributes = params.back().getValueAsInteger();
                            } else {
                                assert(false);
                            }
                        }
                    }

                    namespace modeSettingFunctions {
                        void operate_RESET_MODE(ecma::sequence::base* input) {
                            auto controlSequence = static_cast<ecma::sequence::control<ecma::sequence::parameter::numeric>*>(input);

                            auto params = controlSequence->getParameters();

                            assert(params.size() > 0);

                            // We can now check if the first parameter is a '?' or not, if not, then we proceed to call the base ecma implementation of this function
                            if (params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE) {
                                for (size_t i = 1; i < params.size(); i++) {
                                    auto typed = params[i].getValueAsInteger();

                                    if (ecma::table::contains<modeTypes>(typed)) {
                                        currentStates.decComponents.VT100Components.modes.set({typed, ecma::table::mode::definition::RESET});
                                    }

                                }
                            } else {
                                ecma::sequences::modeSettingFunctions::operate_RESET_MODE(input);
                            }
                        }

                        void operate_SET_MODE(ecma::sequence::base* input) {
                            auto controlSequence = static_cast<ecma::sequence::control<ecma::sequence::parameter::numeric>*>(input);

                            auto params = controlSequence->getParameters();

                            assert(params.size() > 0);

                            // We can now check if the first parameter is a '?' or not, if not, then we proceed to call the base ecma implementation of this function
                            if (params.front().getValueAsInteger() == (uint8_t)ecma::table::parameters::PRIVATE) {
                                for (size_t i = 1; i < params.size(); i++) {
                                    auto typed = params[i].getValueAsInteger();

                                    if (ecma::table::contains<modeTypes>(typed)) {
                                        currentStates.decComponents.VT100Components.modes.set({typed, ecma::table::mode::definition::SET});
                                    }

                                }
                            } else {
                                ecma::sequences::modeSettingFunctions::operate_SET_MODE(input);
                            }
                        }
                    }
                }
            }

            bitMask<features> probe() {
                return {};
            }
        }
    }
}