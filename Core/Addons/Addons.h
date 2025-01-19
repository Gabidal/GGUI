#ifndef _ADDONS_H_
#define _ADDONS_H_

namespace GGUI{
    extern void Init_Addons();

    /**
     * @brief Initializes the inspect tool.
     * @details This function initializes the inspect tool which is a debug tool that displays the number of elements, render time, and event time.
     * @see GGUI::Update_Stats
     */
    extern void Init_Inspect_Tool();
}

#endif