#include "ggui.h"

using namespace GGUI;

int main() 
{
    Window* Main = Init_GGUI();

    Main->Set_Title("Your App UI");

    Text_Field* Txt = new Text_Field("Hello World!");

    Main->Add_Child(Txt);

    // Your code here...
}