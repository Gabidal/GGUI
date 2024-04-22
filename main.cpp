#include "ggui.h"

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){

        GGUI::Terminal_Canvas tem(20, 20, {0, 0});

        GGUI::Sprite s({{"a"}, {"b"}, {"c"}});

        tem.Set(0, 0, s);
    });  

    // Your code here...
    _sleep(UINT32_MAX);
}