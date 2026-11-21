#include "xterm.h"
#include "terminal.h"


namespace GGUI::terminal::xterm {
    ecma::configuration::cellPatch csiPatch(ecma::table::C1::CSI);

    namespace sequences {
        namespace miscellaneousControlFunctions {
            void operate_DEVICE_ATTRIBUTES(ecma::sequence::base* input) {
                auto controlSequence = miscellaneousControlFunctions::DEVICE_ATTRIBUTES.transform(input);

                auto params = controlSequence.getParameters();

                if (params.size() == 1) {   // This is the request for identification, which is already implemented in the ecma.cpp, so we can just reroute into there.
                    ecma::sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(input);
                } else if (params.size() == 2 && params.front().hasSecondaries()) {
                    const auto& [greaterThan, modelNumber] = params.front().getPrimaryValueAndSecondaries().front();

                    if (
                        (ecma::table::parameters)greaterThan == ecma::table::parameters::GREATERTHAN &&
                        (uint8_t)modelNumber == 95  // This magic number comes from here: https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Device-Control-functions
                    ) { currentStates->xtermComponents.enabled = true; }
                } else {
                    assert(false);
                }
            }
        }

        namespace modeSettingFunctions {
            void operate_RESET_PRIVATE_MODE(ecma::sequence::base* input) {
                auto controlSequence = modeSettingFunctions::RESET_PRIVATE_MODE.transform(input);

                auto params = controlSequence.getParameters();

                // Check if the first parameter is private indicator, if not forward to dec
                if (static_cast<ecma::table::parameters>(params.front().getValueAsInteger()) != ecma::table::parameters::PRIVATE) {
                    TODO("Make a proper orphan sequence adopt system for these kind of situation!")
                    if (currentStates->decComponents.isEnabled()) dec::VT100::sequences::modeSettingFunctions::operate_RESET_MODE(input);
                    else ecma::sequences::modeSettingFunctions::operate_RESET_MODE(input);
                }
            }

            void operate_SET_PRIVATE_MODE(ecma::sequence::base* input) {
                auto controlSequence = modeSettingFunctions::SET_PRIVATE_MODE.transform(input);

                auto params = controlSequence.getParameters();

                // Check if the first parameter is private indicator, if not forward to dec
                if (static_cast<ecma::table::parameters>(params.front().getValueAsInteger()) != ecma::table::parameters::PRIVATE) {
                    TODO("Make a proper orphan sequence adopt system for these kind of situation!")
                    if (currentStates->decComponents.isEnabled()) dec::VT100::sequences::modeSettingFunctions::operate_SET_MODE(input);
                    else ecma::sequences::modeSettingFunctions::operate_SET_MODE(input);
                }
            }
        }

        namespace mouseFunctions {
            void operate_MOUSE_PRESS(ecma::sequence::base* input) {
                auto controlSequence = mouseFunctions::MOUSE_PRESS.transform(input);

                auto params = controlSequence.getParameters();
            }

            void operate_MOUSE_RELEASE(ecma::sequence::base*) {

            }
        }
    }

    bool components::verifyExtensions(ecma::sequence::base* parsed) {
        if (
            parsed->getType() == ecma::sequence::types::CSI && 
            (ecma::table::finalWithoutIntermediate)parsed->getPostfix().getFinalByte() == ecma::table::finalWithoutIntermediate::DA
        ) {
            sequences::miscellaneousControlFunctions::operate_DEVICE_ATTRIBUTES(parsed);
        }

        registerPatches();

        return enabled;
    }

    void components::registerPatches() {
        if (!enabled) return;

        auto& pages = currentStates->ecmaComponents.pageManager;
        pages.patch(csiPatch, ecma::configuration::repertoire::C1);
    }
}