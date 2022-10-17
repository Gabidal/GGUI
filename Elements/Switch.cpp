#include "Switch.h"
#include "List_View.h"

namespace GGUI{
    std::vector<UTF> Switch::Render(){

        // States: {O, X} Where: State = 0/1
        UTF Marker(States[State]);

        List_View Container;

        Container.Set_Growth_Direction(Grow_Direction::ROW);

        Container.Add_Child(new Text_Field(Marker.To_String()));

        Container.Add_Child(&Btn);

        return Container.Render();
    }

}