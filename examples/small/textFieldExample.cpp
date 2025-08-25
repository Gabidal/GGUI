#include <GGUI.h>

using namespace GGUI;

int main(){
    GGUI::GGUI(
        node(new listView(
            flowPriority(DIRECTION::ROW) |     // Horizontal list view
            width(1.0f) | height(1.0f) |    // Fullscreen width and height

            node(new textField(
                anchor(ANCHOR::LEFT) |          // Anchor the text left
                width(0.33f) | height(1.0f) | 
                text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.")
            )) | 

            node(new textField(
                anchor(ANCHOR::CENTER) |        // Anchor the text center
                width(0.33f) | height(1.0f) | 
                text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.")
            )) | 

            node(new textField(
                anchor(ANCHOR::RIGHT) |         // Anchor the text right
                width(0.33f) | height(1.0f) | 
                text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.")
            ))
        ))
    );

    while (true){
        // ...
    }
}