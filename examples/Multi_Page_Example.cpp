#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

const char* MENU_NAME = "menu";
const char* CAMPAIGN_NAME = "campaign";
const char* CANVAS_NAME = "canvas";
const char* TEXT_INPUT_NAME = "text_input";
const char* EXIT_NAME = "exit";

GGUI::textField* User_Input;
GGUI::textField* Output;
GGUI::canvas* Map_Canvas;

void inputHandler(string input){
    Output->setText(Output->getText() + "\n" + input);
}

// Switches from the 'From' ID to the 'To' ID
void switchDisplayedElements(const char* From, const char* To){
    GGUI::element* From_Element = INTERNAL::Main->getElement(From);
    GGUI::element* To_Element = INTERNAL::Main->getElement(To);

    // Run these changes in safe mode.
    GGUI::pauseGGUI([From_Element, To_Element](){
        From_Element->display(false);
        To_Element->display(true);
    });
}

node initMenu(){
    return node(new listView(
        flow_priority(DIRECTION::COLUMN) |
        width(1.0f) | height(1.0f) | 
        name(MENU_NAME) | 
        node(new button(
            text(CAMPAIGN_NAME) | 
            on_click([]([[maybe_unused]] element* self){
                self->focus();
                switchDisplayedElements(MENU_NAME, CAMPAIGN_NAME);
                return true;
            })
        )) | 
        node(new button(
            text(EXIT_NAME) | 
            on_click([]([[maybe_unused]] element* self){
                EXIT();
                return true;
            })
        ))
    ));
}

node initCampaign(){
    constexpr int inputFieldHeight = 3;

    return node(new element(
        name(CAMPAIGN_NAME) | display(false) | 
        width(1.0f) | height(1.0f) |

        // Top right canvas
        node(new canvas(
            width(0.5f) | height(0.5f) | position(0.5f, 0) | name(CANVAS_NAME) | enable_border(true)
        )) |

        // Bottom left, text input field
        node(new textField(
            width(0.5f) | height(inputFieldHeight) | 
            name(TEXT_INPUT_NAME) | enable_border(true) | 
            position(STYLES::bottom) | allow_overflow(true) | 
            on_input([](textField* self, char input){
                if (input == '\n'){
                    string text = self->getText();
                    self->setText("");
                    inputHandler(text);
                }
                else{
                    self->setText(self->getText() + input);
                }
            })
        )) | 

        // top left, input history
        node(new textField(
            width(0.5f) | height(0.5f) | enable_border(true) | allow_overflow(true)
        )) 
    ));
}

int main(){
    GGUI::GGUI(
        initMenu() | initCampaign()
    );

    GGUI::INTERNAL::SLEEP(UINT32_MAX);
}