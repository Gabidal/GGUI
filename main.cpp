#include "GGUI.h"
#include <iostream>

using namespace GGUI;
using namespace std;


int main() 
{

    cout << "Downloading..." << endl;

    // [Use GGUIs OUTBOX feature]
    GGUI::Progress_Bar* Download_A = new GGUI::Progress_Bar(GGUI::COLOR::BLUE, GGUI::COLOR::GRAY, 100);

    // Your code here...
    _sleep(UINT32_MAX);
}