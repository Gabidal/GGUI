#ifndef _VULKAN_H_
#define _VULKAN_H_

#include <string>
#include <vector>
#include <iostream>

#if defined(_WIN32)

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>
#include <windows.h>
#include <vulkan_win32.h>

#endif

namespace GGUI{

    namespace Vulkan{
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

            Window_Handle(std::string title, unsigned int width, unsigned int height){
                
                WNDCLASSA info = {};
                info.hInstance = GetModuleHandle(NULL);
                info.lpfnWndProc = DefWindowProc;
                info.lpszClassName = "GGUI_Window";

                if (!RegisterClassA(&info)){
                    std::cout << "Failed to register window class." << std::endl;
                }
                
                Handle =  CreateWindowA(info.lpszClassName, title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, info.hInstance, NULL);

                ShowWindow(Handle, SW_SHOW);
            }

            #endif
        };

    }

    VkSurfaceKHR Surface;

    unsigned int Default_Width = 1000;
    unsigned int Default_Height = 1000;

    void Init(){
        Vulkan::Window_Handle Handle("", Default_Width, Default_Height);

        const char* extension[] = {
            VK_KHR_SURFACE_EXTENSION_NAME
            #if defined(_WIN32)
            ,VK_KHR_WIN32_SURFACE_EXTENSION_NAME
            #endif
        };

        VkInstance Instance = {};

        VkInstanceCreateInfo Instance_Info = {};
        Instance_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        Instance_Info.enabledExtensionCount = sizeof(extension) / sizeof(extension[0]);
        Instance_Info.ppEnabledExtensionNames = extension;

        vkCreateInstance(&Instance_Info, 0, &Instance);

        #if defined(_WIN32)
            
        VkWin32SurfaceCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hinstance = GetModuleHandle(NULL);
        info.hwnd = Handle.Handle;

        vkCreateWin32SurfaceKHR(Instance, &info, NULL, &Surface);

        #else

        #endif

    }

}

#endif