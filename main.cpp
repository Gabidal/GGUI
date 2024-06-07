#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

int main(int Argument_Count, char** Arguments){
    
    GGUI::GGUI([=](){
        
        // Text_Field* left = new Text_Field(
        //     "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
        //     ALIGN::LEFT,
        //     22, 8 
        // );
        // left->Set_Position({1, 1});
        
        // Text_Field* center = new Text_Field(
        //     "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
        //     ALIGN::CENTER,
        //     22, 8 
        // );
        // center->Set_Position({1 + 22 + 1, 1});

        // Text_Field* right = new Text_Field(
        //     "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
        //     ALIGN::RIGHT,
        //     22, 8 
        // );
        // right->Set_Position({1 + 22 + 1 + 22 + 1, 1});


        // Main->Add_Child(left);
        // Main->Add_Child(center);
        // Main->Add_Child(right);

    }, INT32_MAX);

    return 0;
}