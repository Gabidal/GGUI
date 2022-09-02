#include "List_View.h"
#include "../Renderer.h"

//undefine these before algorithm.h is included
#undef min
#undef max
#include <algorithm>

void GGUI::List_View::Add_Child(Element* e){

    int max_width = 0;
    int max_height = 0;

    if (Parent){
        std::pair<int, int> Max_Dimensions = Parent->Get_Fitting_Dimensions(this);

        max_width = Max_Dimensions.first;
        max_height = Max_Dimensions.second;
    }
    else{
        max_width = Max_Width;
        max_height = Max_Height;
    }

    if (Wrap_Overflow){
        Report(
            "Overflow wrapping is not supported!"
        );
    }
    else{
        if (Flow_Priority == Grow_Direction::ROW){
            Height = std::min(std::max(e->Height, Height), max_height);
            Width = std::min(e->Width + Width, max_width);

            e->Position.X = Last_Child_X;
            Last_Child_X += e->Width;
        }
        else{
            Width = std::min(std::max(e->Width, Width), max_height);
            Height = std::min(e->Height + Height, max_width);

            e->Position.Y = Last_Child_Y;
            Last_Child_Y += e->Height;
        }

        e->Parent = this;
        Childs.push_back(e);
        Update_Parent(this);
        GGUI::Update_Frame();
    }
}

// std::vector<GGUI::UTF> GGUI::List_View::Render(){

    

// }

std::string GGUI::List_View::Get_Name(){
    return "List_View";
}

void GGUI::List_View::Update_Parent(Element* deleted){
    if (deleted == this){
        if (Parent)
            Parent->Update_Parent(this);
        return;
    }

    if (Flow_Priority == Grow_Direction::ROW){    
        if (deleted->Height == Height){
            Height = 0;
        }
        //First find the nodes that are after the deleted
        int i = 0;
        for (; i < Childs.size(); i++){
            if (Childs[i] == deleted){
                break;
            }
            //check the first half for new max width
            else {
                if (Childs[i]->Height > Height){
                    Height = Childs[i]->Height;
                }
            }
        }
        for (int j = i + 1; j < Childs.size(); j++){

            Childs[j]->Position.X -= deleted->Width;

            //check on second half for max width
            if (Childs[j]->Height > Height){
                Height = Childs[j]->Height;
            }

        }
    }
    else{
        if (deleted->Width == Width){
            Width = 0;
        }
        //First find the nodes that are after the deleted
        int i = 0;
        for (; i < Childs.size(); i++){
            if (Childs[i] == deleted){
                break;
            }
            //check the first half for new max width
            else {
                if (Childs[i]->Width > Width){
                    Width = Childs[i]->Width;
                }
            }
        }
        for (int j = i + 1; j < Childs.size(); j++){

            Childs[j]->Position.Y -= deleted->Height;

            //check on second half for max width
            if (Childs[j]->Width > Width){
                Width = Childs[j]->Width;
            }

        }
    }
}

