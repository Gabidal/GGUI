#include "ggui.h"

#include <vector>

using namespace std;
using namespace GGUI;

Text_Field* t = nullptr;

int main(int Argument_Count, char** Arguments){
    
    GGUI::GGUI([=](){
        
        t = new Text_Field();
        t->Set_Dimensions(Max_Width -2, Max_Height -2);
        Main->Add_Child(t);

    });

    int i = 0;

    while (true){
        t->Add_Line(to_string(i));

        i++;
    }

    return 0;
}