#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

const char* MENU_NAME = "menu";
const char* CAMPAIGN_NAME = "campaign";
const char* CANVAS_NAME = "canvas";
const char* INPUT_HISTORY = "input_history";
const char* TEXT_INPUT_NAME = "text_input";
const char* EXIT_NAME = "exit";

void inputHandler(string& input){
    GGUI::textField* textHistory = (GGUI::textField*)getRoot()->getElement(INPUT_HISTORY);
    textHistory->setText(textHistory->getText() + "\n" + input);
}

// Switches from the 'From' ID to the 'To' ID
void switchDisplayedElements(const char* From, const char* To){
    GGUI::element* From_Element = getRoot()->getElement(From);
    GGUI::element* To_Element = getRoot()->getElement(To);

    // Run these changes in safe mode.
    GGUI::pauseGGUI([From_Element, To_Element](){
        From_Element->display(false);
        To_Element->display(true);
    });
}

node initMenu(){
    return node(new listView(
        flowPriority(DIRECTION::COLUMN) |
        width(1.0f) | height(1.0f) | 
        name(MENU_NAME) | 
        node(new button(
            text(CAMPAIGN_NAME) | 
            onClick([]([[maybe_unused]] element* self){
                self->focus();
                switchDisplayedElements(MENU_NAME, CAMPAIGN_NAME);
                return true;
            })
        )) | 
        node(new button(
            text(EXIT_NAME) | 
            onClick([]([[maybe_unused]] element* self){
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
            width(0.5f) | height(0.5f) | position(STYLES::top + STYLES::right) | name(CANVAS_NAME) | 
            onDraw([](unsigned int x, unsigned int y){
                return sprite({
                    UTF(' ', {RGB(x*10, x*10, y*10), RGB(y*10, x*10, x*10)}),
                    UTF(' ', {RGB(x*10, y*10, x*10), RGB(y*10, y*10, x*10)}),
                });
            })
        )) |

        // Bottom left, text input field
        node(new textField(
            width(0.5f) | height(inputFieldHeight) | 
            name(TEXT_INPUT_NAME) | enableBorder(true) | 
            position(STYLES::bottom + STYLES::left) | allowOverflow(true) | 
            onInput([](textField* self, char input){
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
            width(0.5f) | height(0.95f) | enableBorder(true) | allowOverflow(true) | name(INPUT_HISTORY)
        )) 
    ));
}

int main(){
    GGUI::GGUI(
        initMenu() | initCampaign()
    );

    GGUI::SLEEP(UINT32_MAX);
}