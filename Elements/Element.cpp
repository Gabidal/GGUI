#include "Element.h"

#include "../Renderer.h"

#include <algorithm>
#include <vector>

#undef min
#undef max


GGUI::Element::Element(std::string Class, unsigned int width, unsigned int height, Element* parent, Coordinates* position){
    Add_Class("default");

    Name = std::to_string((unsigned long long)this);

    if (width != 0)
        Set_Width(width);
    if (height != 0)
        Set_Height(height);

    if (parent){
        Set_Parent(parent);

        Set_Position(position);
    }

    Add_Class(Class);

    Parse_Classes();
}

GGUI::Element::Element() {
    Add_Class("default");
    Name = std::to_string((unsigned long long)this);
    Parse_Classes();

    Dirty.Stain_All();
}

GGUI::Element::Element(std::map<std::string, VALUE*> css, unsigned int width, unsigned int height, Element* parent, Coordinates* position){
    Add_Class("default");
    Parse_Classes();

    bool Previus_Border_State = Has_Border();

    Style = css;

    //Check if the css changed the border state, if so we need to increment or decrement the width & height.
    Show_Border(Has_Border(), Previus_Border_State);

    if (width != 0)
        Set_Width(width);
    if (height != 0)
        Set_Height(height);

    if (parent){
        Set_Parent(parent);

        Set_Position(position);
    }

    Name = std::to_string((unsigned long long)this);
}

//These next constructors are mainly for users to more easily create elements.
GGUI::Element::Element(
    unsigned int width,
    unsigned int height
) : Element(){
    Set_Width(width);
    Set_Height(height);

}

GGUI::Element::Element(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color
) : Element(){
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
}

GGUI::Element::Element(
    unsigned int width,
    unsigned int height,
    RGB text_color,
    RGB background_color,
    RGB border_color,
    RGB border_background_color
) : Element(){
    Set_Width(width);
    Set_Height(height);

    Set_Text_Color(text_color);
    Set_Background_Color(background_color);
    Set_Border_Color(border_color);
    Set_Border_Background_Color(border_background_color);

    Show_Border(true);
}

void GGUI::Element::Inherit_States_From(Element* abstract){
    Focused = abstract->Focused;
    Show = abstract->Show;
}

std::string GGUI::Element::Compose_All_Text_RGB_Values(){
    if (Focused){
        return At<RGB_VALUE>(STYLES::Focus_Text_Color)->Value.Get_Over_Head(true) + 
        At<RGB_VALUE>(STYLES::Focus_Text_Color)->Value.Get_Colour() + 
        Constants::END_COMMAND + 
        At<RGB_VALUE>(STYLES::Focus_Background_Color)->Value.Get_Over_Head(false) + 
        At<RGB_VALUE>(STYLES::Focus_Background_Color)->Value.Get_Colour() +
        Constants::END_COMMAND;
    }
    else{
        return At<RGB_VALUE>(STYLES::Text_Color)->Value.Get_Over_Head(true) + 
        At<RGB_VALUE>(STYLES::Text_Color)->Value.Get_Colour() + 
        Constants::END_COMMAND + 
        At<RGB_VALUE>(STYLES::Background_Color)->Value.Get_Over_Head(false) + 
        At<RGB_VALUE>(STYLES::Background_Color)->Value.Get_Colour() +
        Constants::END_COMMAND;
    }
}

std::string GGUI::Element::Compose_All_Border_RGB_Values(){
    if (Focused){
        return At<RGB_VALUE>(STYLES::Focus_Border_Color)->Value.Get_Over_Head(true) + 
        At<RGB_VALUE>(STYLES::Focus_Border_Color)->Value.Get_Colour() + 
        Constants::END_COMMAND + 
        At<RGB_VALUE>(STYLES::Focus_Border_Background_Color)->Value.Get_Over_Head(false) + 
        At<RGB_VALUE>(STYLES::Focus_Border_Background_Color)->Value.Get_Colour() +
        Constants::END_COMMAND;
    }
    else{
        return At<RGB_VALUE>(STYLES::Border_Colour)->Value.Get_Over_Head(true) + 
        At<RGB_VALUE>(STYLES::Border_Colour)->Value.Get_Colour() + 
        Constants::END_COMMAND + 
        At<RGB_VALUE>(STYLES::Border_Background_Color)->Value.Get_Over_Head(false) + 
        At<RGB_VALUE>(STYLES::Border_Background_Color)->Value.Get_Colour() +
        Constants::END_COMMAND;
    }
}

//End of user constructors.

GGUI::RGB GGUI::Element::Get_RGB_Style(std::string style_name){
    return At<RGB_VALUE>(style_name)->Value;
}

int GGUI::Element::Get_Number_Style(std::string style_name){
    return At<NUMBER_VALUE>(style_name)->Value;
}

bool GGUI::Element::Get_Bool_Style(std::string style_name){
    return At<BOOL_VALUE>(style_name)->Value;
}

GGUI::VALUE* GGUI::Element::Get_Style(std::string style_name){
    return At<VALUE>(style_name);
}

void GGUI::Element::Set_Style(std::string style_name, VALUE* value){
    *At<VALUE>(style_name) = *value;
}

void GGUI::Element::Parse_Classes(){

    bool Remember_To_Affect_Width_And_Height_Because_Of_Border = false;
    bool Previus_Border_Value = At<BOOL_VALUE>(STYLES::Border)->Value;

    //Go through all classes and their styles and accumulate them.
    for(auto& Class : Classes){

        // The class wantwd has not been yet constructed.
        if (GGUI::Classes.find(Class) == GGUI::Classes.end()){
            Dirty.Dirty(STAIN_TYPE::CLASS);
        }

        std::map<std::string, VALUE*> Current = GGUI::Classes[Class];

        for (auto& Current_Style : Current){

            if (Current_Style.first == GGUI::STYLES::Border){
                Remember_To_Affect_Width_And_Height_Because_Of_Border = true;
            }

            //Classes only affect globally, but local styles are priority.
            if (Style.find(Current_Style.first) == Style.end()){
                Style[Current_Style.first] = Current_Style.second->Copy();
            }
        }
    }

    if (Remember_To_Affect_Width_And_Height_Because_Of_Border){
        Show_Border(At<BOOL_VALUE>(STYLES::Border)->Value, Previus_Border_Value);
    }
}


std::map<std::string, GGUI::VALUE*> GGUI::Element::Get_Style(){
    return Style;
}

void GGUI::Element::Set_Style(std::map<std::string, VALUE*> css){
    Style = css;

    Update_Frame();
}

void GGUI::Element::Add_Class(std::string class_name){
    if(Class_Names.find(class_name) != Class_Names.end()){
        Classes.push_back(Class_Names[class_name]);
    }
    else{
        Classes.push_back(GGUI::Get_Free_Class_ID(class_name));
    }
}

bool GGUI::Element::Has(std::string s){
    //first convert the string to the ID
    int id = Class_Names[s];

    //then check if the element has the class
    for(int i = 0; i < Classes.size(); i++){
        if(Classes[i] == id){
            return true;
        }
    }

    return false;
}

void GGUI::Element::Show_Border(bool b){
    if (b != At<BOOL_VALUE>(STYLES::Border)->Value){
        At<BOOL_VALUE>(STYLES::Border)->Value = b;
        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

void GGUI::Element::Show_Border(bool b, bool Previus_State){
    if (b != Previus_State){
        At<BOOL_VALUE>(STYLES::Border)->Value = b;
        Dirty.Dirty(STAIN_TYPE::EDGE);
        Update_Frame();
    }
}

bool GGUI::Element::Has_Border(){
    return At<BOOL_VALUE>(STYLES::Border)->Value;
}

void GGUI::Element::Add_Child(Element* Child){
    // Dont need to check both sides of the bordering, because the element only grows towards. to the bottom right corner

    bool This_Has_Border = Has_Border();
    bool Child_Has_Border = Child->Has_Border();

    if (
        Child->Position.X + Child->Width > (Width - (This_Has_Border - Child_Has_Border) * This_Has_Border) || 
        Child->Position.Y + Child->Height > (Height - (This_Has_Border - Child_Has_Border) * This_Has_Border)
    ){
        if (Child->Resize_To(this) == false){

            GGUI::Report(
                "Window exeeded bounds\n "
                "Starts at: {" + std::to_string(Child->Position.X) + ", " + std::to_string(Child->Position.Y) + "}\n "
                "Ends at: {" + std::to_string(Child->Position.X + Child->Width) + ", " + std::to_string(Child->Position.Y + Child->Height) + "}\n "
                "Max is at: {" + std::to_string(Width) + ", " + std::to_string(Height) + "}\n "
            );

            return;
        }
    }

    Dirty.Dirty(STAIN_TYPE::DEEP);
    Child->Parent = this;

    Element_Names.insert({Child->Name, Child});

    Childs.push_back(Child);
    Update_Frame();
}

void GGUI::Element::Set_Childs(std::vector<Element*> childs){
    bool tmp = Pause_Render;
    if (Pause_Render)
        Pause_Renderer();

    for (auto& Child : childs){
        Add_Child(Child);
    }

    if (tmp)
        Resume_Renderer();
}

std::vector<GGUI::Element*>& GGUI::Element::Get_Childs(){
    return Childs;
}

bool GGUI::Element::Remove(Element* handle){
    for (int i = 0; i < Childs.size(); i++){
        if (Childs[i] == handle){
            //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
            if (Focused_On == Childs[i]){
                Mouse = Childs[i]->Parent->Position;
            }

            Childs.erase(Childs.begin() + i);
            Update_Parent(handle);
            return true;
        }
    }
    return false;
}

void GGUI::Element::Update_Parent(Element* New_Element){
    //normally elements dont do anything
    if (!New_Element->Is_Displayed()){
        Dirty.Stain_All();
    }

    if (Parent){
        Parent->Update_Parent(New_Element);
    }
    else{
        Dirty.Stain_All();
        Update_Frame(); //the most top (Main) will not flush all the updates to render.
    }
}

void GGUI::Element::Display(bool f){
    // Check if the to be displayed is true and the element wasnt already displayed.
    if (f != Show){
        if (f){
            Dirty.Stain_All();
            Show = true;
        }
        else{
            Dirty.Stain_All();
            Show = false;
        }
    }

    Update_Frame();
}

bool GGUI::Element::Is_Displayed(){
    return Show;
}

bool GGUI::Element::Remove(int index){
    if (index > Childs.size() - 1){
        return false;
    }
    Element* tmp = Childs[index];

    //If the mouse if focused on this about to be deleted element, change mouse position into it's parent Position.
    if (Focused_On == tmp){
        Mouse = tmp->Parent->Position;
    }

    Childs.erase(Childs.begin() + index);
    Update_Parent(tmp);
    return true;
}

void GGUI::Element::Remove(){
    if (Parent){
        //tell the parent what is about to happen.
        //you need to update the parent before removing the child, 
        //otherwise the code cannot erase it when it is not found!
        Parent->Remove(this);
    }
    else{
        Report(
            std::string("Cannot remove ") + Get_Name() + std::string(", with no parent\n")
        );
    }
}

void GGUI::Element::Set_Dimensions(int width, int height){
    if (width != Width || height != Height){    
        Width = width;
        Height = height;
        Dirty.Stain_All();
        Update_Frame();
    }
}

int GGUI::Element::Get_Width(){
    return Width;
}

int GGUI::Element::Get_Height(){
    return Height;
}

void GGUI::Element::Set_Width(int width){
    if (width != Width){
        Width = width;
        Dirty.Stain_All();
        if (Parent)
            Update_Parent(this);
        else
            Update_Frame();
    }
}

void GGUI::Element::Set_Height(int height){
    if (height != Height){
        Height = height;
        Dirty.Stain_All();
        if (Parent)
            Update_Parent(this);
        else
            Update_Frame();
    }
}

void GGUI::Element::Set_Position(Coordinates c){
    Position = c;
    if (Parent)
        Parent->Dirty.Dirty(STAIN_TYPE::STRECH);
    Update_Frame();
}

void GGUI::Element::Set_Position(Coordinates* c){
    if (c){
        Position = *c;
        if (Parent)
            Parent->Dirty.Dirty(STAIN_TYPE::STRECH);
        Update_Frame();
    }
}

GGUI::Coordinates GGUI::Element::Get_Position(){
    return Position;
}

GGUI::Coordinates GGUI::Element::Get_Absolute_Position(){
    Coordinates Result = {0, 0};
    
    Element* current_element = this;
    Result = current_element->Position;
    current_element = current_element->Parent;

    while (current_element != nullptr){
        Result.X += current_element->Position.X;
        Result.Y += current_element->Position.Y;
        Result.Z += current_element->Position.Z;

        current_element = current_element->Parent;
    }

    return Result;
}

std::pair<unsigned int, unsigned int> GGUI::Element::Get_Fitting_Dimensions(Element* child){
    GGUI::Element tmp = *child;
    tmp.Style.clear();

    tmp.Width = 0;
    tmp.Height = 0;

    int Border_Size = (Has_Border() * 2) - (child->Has_Border() * 2);

    while (true){
        if (tmp.Position.X + tmp.Width < Width - Border_Size){
            tmp.Width++;
        }
        
        if (tmp.Position.Y + tmp.Height < Height - Border_Size){
            tmp.Height++;
        }
        else if (tmp.Position.X + tmp.Width >= Width - Border_Size && tmp.Position.X + tmp.Height >= Height - Border_Size){
            break;
        }
        
        for (auto c : Childs){
            if (Collides(child, c)){
                //there are already other childs occupying this area so we can stop here.
                return {tmp.Width, tmp.Height};
            }
        }

    }

    return {tmp.Width, tmp.Height};
}

void GGUI::Element::Set_Background_Color(RGB color){
    At<RGB_VALUE>(STYLES::Background_Color)->Value = color;
    if (At<RGB_VALUE>(STYLES::Border_Background_Color)->Value == At<RGB_VALUE>(STYLES::Background_Color)->Value)
        At<RGB_VALUE>(STYLES::Border_Background_Color)->Value = color;
        
    Dirty.Dirty(STAIN_TYPE::COLOR);
    
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Background_Color(){
    return At<RGB_VALUE>(STYLES::Background_Color)->Value;
}

void GGUI::Element::Set_Border_Color(RGB color){
    At<RGB_VALUE>(STYLES::Border_Colour)->Value = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Color(){
    return At<RGB_VALUE>(STYLES::Border_Colour)->Value;
}

void GGUI::Element::Set_Border_Background_Color(RGB color){
    At<RGB_VALUE>(STYLES::Border_Background_Color)->Value = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Border_Background_Color(){
    return At<RGB_VALUE>(STYLES::Border_Background_Color)->Value;
}

void GGUI::Element::Set_Text_Color(RGB color){
    At<RGB_VALUE>(STYLES::Text_Color)->Value = color;
    Dirty.Dirty(STAIN_TYPE::COLOR);
    Update_Frame();
}

GGUI::RGB GGUI::Element::Get_Text_Color(){
    return At<RGB_VALUE>(STYLES::Text_Color)->Value;
}

//Returns nested buffer of AST window's
std::vector<GGUI::UTF> GGUI::Element::Render(){
    std::vector<GGUI::UTF> Result = Render_Buffer;

    //if inned children have changed whitout this changing, then this will trigger.
    if (Children_Changed()){
        Dirty.Dirty(STAIN_TYPE::DEEP);
    }
            
    if (Dirty.is(STAIN_TYPE::CLEAN))
        return Result;

    if (Dirty.is(STAIN_TYPE::CLASS)){
        Parse_Classes();

        Dirty.Clean(STAIN_TYPE::CLASS);
    }

    if (Dirty.is(STAIN_TYPE::STRECH)){
        Result.clear();
        Result.resize(Width * Height);
        Dirty.Clean(STAIN_TYPE::STRECH);

        Dirty.Dirty(STAIN_TYPE::COLOR | STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);
    }

    //Apply the color system to the resized result list
    if (Dirty.is(STAIN_TYPE::COLOR))
        Apply_Colors(this, Result);

    //This will add the child windows to the Result buffer
    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);
        for (auto c : this->Get_Childs()){
            if (!c->Is_Displayed())
                continue;

            if (c->Has_Border() && Has_Border())
                Dirty.Dirty(STAIN_TYPE::EDGE | STAIN_TYPE::DEEP);

            Nest_Element(this, c, Result, c->Render());
        }
    }

    //This will add the borders if nessesary and the title of the window.
    if (Dirty.is(STAIN_TYPE::EDGE))
        Add_Overhead(this, Result);

    // This will calculate the connecting borders.
    if (Dirty.is(STAIN_TYPE::DEEP)){
        Dirty.Clean(STAIN_TYPE::DEEP);
        for (auto A : this->Get_Childs()){
            for (auto B : this->Get_Childs()){
                if (A == B)
                    continue;

                if (!A->Is_Displayed() || !A->Has_Border() || !B->Is_Displayed() || !B->Has_Border())
                    continue;

                Post_Process_Borders(A, B, Result);
            }

            Post_Process_Borders(this, A, Result);
        }
    }

    Render_Buffer = Result;

    return Result;
}

//These are just utility functions for internal purposes, dont need to sed Dirty.
void GGUI::Element::Apply_Colors(Element* w, std::vector<UTF>& Result){
    Dirty.Clean(STAIN_TYPE::COLOR);

    for (auto& utf : Result){
        utf.Pre_Fix = w->Compose_All_Text_RGB_Values();
        
        if (utf.Pre_Fix != "")
            utf.Post_Fix = Constants::RESET_Text_Color + Constants::RESET_Back_Ground_Color;
    }
}

void GGUI::Element::Add_Overhead(GGUI::Element* w, std::vector<GGUI::UTF>& Result){
    Dirty.Clean(STAIN_TYPE::EDGE);
    
    if (!w->Has_Border())
        return;

    for (int y = 0; y < Height; y++){
        for (int x = 0; x < Width; x++){
            //top left corner
            if (y == 0 && x == 0){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::TOP_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //top right corner
            else if (y == 0 && x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::TOP_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom left corner
            else if (y == Height - 1 && x == 0){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_LEFT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //bottom right corner
            else if (y == Height - 1 && x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::BOTTOM_RIGHT_CORNER, w->Compose_All_Border_RGB_Values());
            }
            //The roof border
            else if (y == 0 || y == Height - 1){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::HORIZONTAL_LINE, w->Compose_All_Border_RGB_Values());
            }
            //The left border
            else if (x == 0 || x == Width - 1){
                Result[y * Width + x] = GGUI::UTF(SYMBOLS::VERTICAL_LINE, w->Compose_All_Border_RGB_Values());
            }
        }
    }
}

void GGUI::Element::Nest_Element(GGUI::Element* Parent, GGUI::Element* Child, std::vector<GGUI::UTF>& Parent_Buffer, std::vector<GGUI::UTF> Child_Buffer){
    
    unsigned int Max_Allowed_Height = Parent->Height - (Parent->Has_Border() - Child->Has_Border()) * Parent->Has_Border();             //remove bottom borders from calculation
    unsigned int Max_Allowed_Width = Parent->Width - (Parent->Has_Border() - Child->Has_Border()) * Parent->Has_Border();              //remove right borders from calculation

    unsigned int Min_Allowed_Height = 0 + (Parent->Has_Border() - Child->Has_Border()) * Parent->Has_Border();                        //add top borders from calculation
    unsigned int Min_Allowed_Width = 0 + (Parent->Has_Border() - Child->Has_Border()) * Parent->Has_Border();                         //add left borders from calculation

    unsigned int Child_Start_Y = Min_Allowed_Height + Child->Position.Y;
    unsigned int Child_Start_X = Min_Allowed_Width + Child->Position.X;

    unsigned int Child_End_Y = Min(Child_Start_Y + Child->Height, Max_Allowed_Height);
    unsigned int Child_End_X = Min(Child_Start_X + Child->Width, Max_Allowed_Width);

    for (int y = Child_Start_Y; y < Child_End_Y; y++){
        for (int x = Child_Start_X; x < Child_End_X; x++){
            Parent_Buffer[y * Width + x] = Child_Buffer[(y - Child_Start_Y) * Child->Width + (x - Child_Start_X)];
        }
    }
}

inline bool Allow(GGUI::Coordinates index, GGUI::Element* parent){
    // checks if the index is out of bounds
    if (index.X < 0 || index.Y < 0 || index.X >= parent->Get_Width() || index.Y >= parent->Get_Height())
        return false;

    return true;
}

inline GGUI::UTF& From(GGUI::Coordinates index, std::vector<GGUI::UTF>& Parent_Buffer, GGUI::Element* Parent){
    return Parent_Buffer[index.Y * Parent->Get_Width() + index.X];
}

void GGUI::Element::Post_Process_Borders(Element* A, Element* B, std::vector<UTF>& Parent_Buffer){

    // We only need to calculate the childs points in which they intersect with the parent borders.
    // At these intersecting points of border we will construct a bit mask that portraits the connections the middle point has.
    // With the calculated bit mask we can fetch from the 'SYMBOLS::Border_Identifiers' the right border string.

    // First calculate if the childs borders even touch the parents borders.
    // If not, there is no need to calculate anything.

    // First calculate if the child is outside the parent.
    if (B->Position.X + B->Width < A->Position.X || B->Position.X > A->Position.X + A->Width || B->Position.Y + B->Height < A->Position.Y || B->Position.Y > A->Position.Y + A->Height)
        return;

    // Now calculate if the child is inside the parent.
    if (B->Position.X > A->Position.X && B->Position.X + B->Width < A->Position.X + A->Width && B->Position.Y > A->Position.Y && B->Position.Y + B->Height < A->Position.Y + A->Height)
        return;


    // Now that we are here it means the both boxes interlace eachother.
    // We will calculate the hitting points by drawing segments from corner to corner and then comparing one segments x to other segments y, and so forth.

    // two nested loops rotating the x and y usages.
    // store the line x,y into a array for the nested loops to access.
    std::vector<unsigned int> Vertical_Line_X_Coordinates = {
        
        B->Position.X,
        A->Position.X,
        B->Position.X + B->Width - 1,
        A->Position.X + A->Width - 1,

                
        A->Position.X,
        B->Position.X,
        A->Position.X + A->Width - 1,
        B->Position.X + B->Width - 1

    };

    std::vector<unsigned int> Horizontal_Line_Y_Cordinates = {
        
        A->Position.Y,
        B->Position.Y + B->Height - 1,
        A->Position.Y,
        B->Position.Y + B->Height - 1,

        B->Position.Y,
        A->Position.Y + A->Height - 1,
        B->Position.Y,
        A->Position.Y + A->Height - 1,

    };

    int Participants_Count = 2;
    int Box_Axes_Count = 2;

    std::vector<Coordinates> Crossing_Indicies;

    // Go through singular box
    for (int Box_Index = 0; Box_Index < Horizontal_Line_Y_Cordinates.size(); Box_Index++){
        // Now just pair the indicies from the two lists.
        Crossing_Indicies.push_back(
            // First pair
            Coordinates(
                Vertical_Line_X_Coordinates[Box_Index],
                Horizontal_Line_Y_Cordinates[Box_Index]
            )
        );
    }

    // Now that we have the crossing points we can start analyzing the ways they connect to construct the bit masks.
    for (auto c : Crossing_Indicies){

        Coordinates Above = { c.X, c.Y - 1 };
        Coordinates Below = { c.X, c.Y + 1 };
        Coordinates Left = { c.X - 1, c.Y };
        Coordinates Right = { c.X + 1, c.Y };

        unsigned int Current_Masks = 0;

        if (Allow(Above, this) && From(Above, Parent_Buffer, this).Unicode == SYMBOLS::VERTICAL_LINE)
            Current_Masks |= SYMBOLS::CONNECTS_UP;

        if (Allow(Below, this) && From(Below, Parent_Buffer, this).Unicode == SYMBOLS::VERTICAL_LINE)
            Current_Masks |= SYMBOLS::CONNECTS_DOWN;

        if (Allow(Left, this) && From(Left, Parent_Buffer, this).Unicode == SYMBOLS::HORIZONTAL_LINE)
            Current_Masks |= SYMBOLS::CONNECTS_LEFT;

        if (Allow(Right, this) && From(Right, Parent_Buffer, this).Unicode == SYMBOLS::HORIZONTAL_LINE)
            Current_Masks |= SYMBOLS::CONNECTS_RIGHT;

        From(c, Parent_Buffer, this).Unicode = SYMBOLS::Border_Identifiers[Current_Masks];
    }
}

//End of utility functions.

void GGUI::Element::On_Click(std::function<void(GGUI::Event* e)> action){
    Action* a = new Action(
        Constants::ENTER,
        [=](GGUI::Event* e){
            if (Collides(this, Mouse)){
                action(e);

                //action succesfully executed.
                return true;
            }
            //action failed.
            return false;
        },
        this
    );
    GGUI::Event_Handlers.push_back(a);
}

GGUI::Element* GGUI::Element::Copy(){
    Element* new_element = new Element();

    *new_element = *this;

    //now also update the event handlers.
    for (auto& e : GGUI::Event_Handlers){

        if (e->Host == this){
            //copy the event and make a new one
            Action* new_action = new Action(*e);

            //update the host
            new_action->Host = new_element;

            //add the new action to the event handlers list
            GGUI::Event_Handlers.push_back(new_action);
        }

    }

    return new_element;
}

bool GGUI::Element::Children_Changed(){
    if (Dirty.Type != STAIN_TYPE::CLEAN)
        return true;

    for (auto& e : Childs){
        if (e->Children_Changed())
            return true;
    }

    return false;
}

void GGUI::Element::Set_Name(std::string name){
    Name = name;

    Element_Names[name] = this;
}

GGUI::Element* GGUI::Element::Get_Element(std::string name){
    Element* Result = nullptr;

    if (Element_Names.find(name) != Element_Names.end()){
        Result = Element_Names[name];
    }

    return Result;
}

