#include "ggui.h"

using namespace GGUI;

bool DisableOthers(radioButton* keepOn){
    keepOn->setState(true);

    if (!keepOn->getParent())
        return true;

    for (auto* c : keepOn->getParent()->getElements<radioButton>()){
        if (c != keepOn)
            c->setState(false);
    }

    return true;
}

int main(){
    GGUI::GGUI(
        node(new listView(
            flow_priority(DIRECTION::COLUMN) | enable_border(true) |
            childs({
                new radioButton(
                    text("A") | on_click([](element* self){ return DisableOthers((radioButton*)self); })
                ),
                new radioButton(
                    text("B") | on_click([](element* self){ return DisableOthers((radioButton*)self); })
                ),
                new radioButton(
                    text("C") | on_click([](element* self){ return DisableOthers((radioButton*)self); })
                )
            })
        ))
    );

    while(true){
        // ...
    }
}