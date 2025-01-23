#ifndef _ADDONS_H_
#define _ADDONS_H_

namespace GGUI{
    
    /**
     * @brief Initializes all addons and adds them to the main internal structure.
     *
     * This function first calls the initializer for the inspect tool addon.
     * After all addons are loaded, it iterates through the list of addons
     * and adds each one to the main internal structure.
     */
    extern void initAddons();

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::Update_Stats
     */
    extern void initInspectTool();
}

#endif