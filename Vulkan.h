#ifndef _VULKAN_H_
#define _VULKAN_H_

#if defined(_WIN32)

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_PROTOTYPES
#include <vulkan/vulkan.h>
#include <windows.h>

#include <vulkan/vulkan_win32.h>

#endif

#include <string>
#include <vector>
#include <iostream>


namespace GGUI{

    class Window_Handle;
    class Graphics_Device;
    class Swap_Chain;

    inline VkSurfaceKHR Surface;
    inline VkInstance Instance;
    inline Graphical_Device Selected_Device;
    inline VkSurfaceFormatKHR Selected_Surface_Format;
    inline VkPresentModeKHR Selected_Present_Mode;
    inline VkRenderPass Render_Pass;
    inline Swap_Chain Swapchain;

    inline std::vector<Graphical_Device> Graphical_Devices;
    inline std::vector<VkSurfaceFormatKHR> Surface_Formats;
    inline std::vector<VkPresentModeKHR> Present_Modes;

    inline unsigned int Default_Width = 1000;
    inline unsigned int Default_Height = 1000;

    #if defined(_WIN32)
        #include <windows.h>
    #endif

    class Window_Handle{
    public:
        unsigned int Width = 0;
        unsigned int Height = 0;

        #if defined(_WIN32)
        
        union{
            HWND Handle;
        };

        Window_Handle(std::string title, unsigned int width, unsigned int height);

        #endif
    };

    class Graphical_Device{
    public:

        VkPhysicalDevice Device;
        std::vector<VkQueueFamilyProperties> Queue_Families;

        Graphical_Device(VkPhysicalDevice device);

    };

    class Swap_Chain{
    public:
        VkSwapchainKHR Swapchain;

        std::vector<VkFramebuffer> Framebuffers;

        unsigned int Width;
        unsigned int Height;

        Swap_Chain(unsigned int width, unsigned int height);
    };

    enum Shader_Type{
        Vertex      = 1,
        Fragment    = 16,
        Compute     = 32,
        Geometry    = 8
    };

    class Shader{
    public:
        VkShaderModule	Module;
        VkDevice		Device;
        Shader_Type		Type;

        const char		*mEntrypoint;

        Shader(std::string path, Shader_Type type);
    };

    // Searches for all graphical devices, uses the first by default.
    extern void Init_Graphical_Devices();

    // Gets all the available surface formats.
    extern void Init_Surface_Formats();

    // Gets all the available present modes.
    extern void Init_Present_Modes();

    // Creates the render pass.
    extern void Init_Render_Pass();

    // Setups the VkSurfaceKHR
    extern void Init();


}

#endif