#include <GGUI.h>
#include "units/utils.h"
#include "units/superStringSuite.h"
#include "units/ansiSuite.h"
#include "units/colorSuite.h"
#include "units/elementSuite.h"

int main() {
    std::cout << tester::utils::colorText("GGUI Framework Test Suite\n", GGUI::COLOR::BLUE);
    std::cout << "========================================\n";
    std::cout << "Testing GGUI components and functionality...\n\n";
    
    try {
        tester::utils::run_all_tests({
            new tester::superStringSuite(),
            new tester::ansiSuite(),
            new tester::colorSuite(),
            new tester::elementSuite(),
        });
    } catch (const std::exception& e) {
        std::cout << tester::utils::colorText("Test suite crashed with exception: ", GGUI::COLOR::RED) << e.what() << "\n";
        return 2;
    } catch (...) {
        std::cout << tester::utils::colorText("Test suite crashed with unknown exception!\n", GGUI::COLOR::RED);
        return 2;
    }

    return 0;
}