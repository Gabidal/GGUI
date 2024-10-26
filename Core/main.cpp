#include "ggui.h"

#include <math.h>

using namespace GGUI;

int main() 
{
    GGUI::GGUI([&](){
        
        GGUI::List_View* Inspect = new GGUI::List_View(Styling(
            width(Main->Get_Width() / 2) | height(Main->Get_Height()) | 
            text_color(COLOR::RED) | background_color(COLOR::BLUE) 
        ));

        Inspect->Set_Flow_Direction(DIRECTION::COLUMN);
        Inspect->Show_Border(false);
        Inspect->Set_Position({
            Main->Get_Width() - (Main->Get_Width() / 2),
            0,
            INT32_MAX - 1,
        });
        Inspect->Set_Opacity(0.8f);
        Inspect->Set_Name("123Inspect");

        Main->Add_Child(Inspect);
        
        // Add a count for how many UTF are being streamed.
        Text_Field* Stats = new Text_Field(
            "Encode: " + std::to_string(Abstract_Frame_Buffer.size()) + "\n" + 
            "Decode: " + std::to_string(Frame_Buffer.size()) + "\n" +
            "Elements: " + std::to_string(Main->Get_All_Nested_Elements().size()) + "\n" +
            "Render delay: " + std::to_string(Render_Delay) + "ms\n" +
            "Event delay: " + std::to_string(Event_Delay) + "ms",
            Styling(
                align(ALIGN::LEFT) | width(Inspect->Get_Width()) | height(5) | text_color(COLOR::CYAN)
            )
        );
        Stats->Set_Name("123STATS");

        Inspect->Add_Child(Stats);

        // Add the error logger kidnapper:
        Window* Error_Logger_Kidnapper = new Window(
            "LOGasd: ",
            Styling(
                width(Inspect->Get_Width()) | height(Inspect->Get_Height() / 2) |
                text_color(GGUI::COLOR::RED) | background_color(GGUI::COLOR::BLACK) |
                border_color(GGUI::COLOR::RED) | border_background_color(GGUI::COLOR::BLACK)
            )
        );

        Error_Logger_Kidnapper->Set_Name("ERROR_LOGGERasdasd");
        Error_Logger_Kidnapper->Allow_Overflow(true);

        Inspect->Add_Child(Error_Logger_Kidnapper);
        Inspect->Display(true);


    }, INT32_MAX);

    GGUI::Exit();
}