#include "ggui.h"

using namespace std;
using namespace GGUI;

int main(){
    textField* t = new textField(
        text(
    "A AA AAA AAAA AAAAA AAAAAA AAAAAAA AAAAAAAA \n"
    "B BB BBB BBBB BBBBB BBBBBB BBBBBBB BBBBBBBB \n"
    "C CC CCC CCCC CCCCC CCCCCC CCCCCCC CCCCCCCC \n"
        ) | 

        on_click([](element* self){
            self->remove();

            return true;
        })
    );
    

    GGUI::GGUI(
        node(new listView(
            node(t) | node(t->copy())
        ))
    );

    while(true){ 
        //Your programm here :D
    }
}