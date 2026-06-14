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
                            currentStates.components.activePresentationPosition.y--;
                        }

                        void operate_CURSOR_DOWN(ecma::sequence::base* /*ignored*/) {
                            currentStates.components.activePresentationPosition.y++;
                        }

                        void operate_CURSOR_LEFT(ecma::sequence::base* /*ignored*/) {
                            currentStates.components.activePresentationPosition.x--;
                        }

                        void operate_CURSOR_RIGHT(ecma::sequence::base* /*ignored*/) {
                            currentStates.components.activePresentationPosition.x++;
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