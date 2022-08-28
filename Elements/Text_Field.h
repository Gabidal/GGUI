#ifndef _TEXT_FIELD_H_
#define _TEXT_FIELD_H_

#include <string>
#include <vector>

#include "Element.h"

namespace GGUI{
    enum class TEXT_LOCATION{
        CENTER,
        LEFT,
        RIGHT,
    };

    class Text_Field : public Element{
    public:
        std::string Data = "";
        TEXT_LOCATION Text_Position = TEXT_LOCATION::LEFT;
        
        static std::pair<int, int> Get_Text_Dimensions(std::string text); 

        std::vector<UTF> Render() override;

        static void Center_Text(std::vector<UTF>& Text, int width, int height);
    };
}

#endif