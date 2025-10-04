#include "ggui.h"

using namespace GGUI;

int main(int argc, char* argv[]){
    
    switchBox SB(
        visualState("0", "1") | onClick([](element*){ return true; }) // Enable clicking and enter functions
    );

    SB.compile();

}
