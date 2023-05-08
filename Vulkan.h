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
#include <atomic>


namespace GGUI{
    namespace VULKAN{
        class Window_Handle;
        class Graphical_Device;
        class Swap_Chain;
        class Vertex;
        class Shader;

        extern VkSurfaceKHR             Surface;
        extern VkInstance               Instance;
        extern Graphical_Device         Selected_Device;
        extern VkSurfaceFormatKHR       Selected_Surface_Format;
        extern VkPresentModeKHR         Selected_Present_Mode;
        extern VkRenderPass             Render_Pass;
        extern Swap_Chain               Swapchain;
        extern VkPipeline               Pipeline;
        extern VkPipelineLayout         Pipeline_Layout;
        extern Shader                   Vertex_Shader;
        extern Shader                   Fragment_Shader;
        extern VkCommandPool            Command_Pool;
        extern VkDebugUtilsMessengerEXT Debug_Messenger;
        extern VkSemaphore              Image_Available_Semaphore;
        extern VkSemaphore              Rendering_Finished_Semaphore;
        extern VkFence                  Frame_Fence;

        extern std::atomic_bool         Pause_Vulkan;

        extern std::vector<Graphical_Device> Graphical_Devices;
        extern std::vector<VkSurfaceFormatKHR> Surface_Formats;
        extern std::vector<VkPresentModeKHR> Present_Modes;
        extern std::vector<VkCommandBuffer> Command_Buffers;
        extern std::vector<Vertex> Vertices;

        extern unsigned int Default_Width;
        extern unsigned int Default_Height;

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

            #endif

            Window_Handle(){}
            
            Window_Handle(std::string title, unsigned int width, unsigned int height);

            void Window_Events();
        };

        class Graphical_Device{
        public:

            VkPhysicalDevice Physical_Device;
            VkDevice Device;
            std::vector<VkQueueFamilyProperties> Queue_Families;
            int Queue_Index = -1;
            VkSurfaceCapabilitiesKHR Surface_Capabilities;
            VkQueue Present_Queue;
            VkQueue Graphics_Queue;

            Graphical_Device(VkPhysicalDevice phy_device);

            void Get_Physical_Device_Capabilities();

            // This is mainly generated just for the global variables to work.
            Graphical_Device(){}

        };

        class Swap_Chain{
        public:
            VkSwapchainKHR Swapchain;

            std::vector<VkFramebuffer> Framebuffers;

            std::vector<VkImageView> swapchain_image_views;

            unsigned int Width;
            unsigned int Height;

            Swap_Chain(unsigned int width, unsigned int height);

            // This is mainly generated just for the global variables to work.
            Swap_Chain(){}
        };

        class Shader{
        public:
            VkShaderModule Module;
            VkShaderStageFlagBits	Type;

            std::string Entry_Point_Name = "main";

            Shader(std::string path, VkShaderStageFlagBits type);

            // This is mainly generated just for the global variables to work.
            Shader(){}
        };

        enum class Priority{
            High,
            Medium,
            Low
        };

        class Vector2{
        public:
            float x;
            float y;

            Vector2(float x, float y): x(x), y(y){}
            
            // This is mainly generated just for the global variables to work.
            Vector2(){}
        };

        class Vector3{
        public:
            float x = 0;
            float y = 0;
            float z = 0;

            Vector3(float x, float y, float z = 0): x(x), y(y), z(z){}
            
            // This is mainly generated just for the global variables to work.
            Vector3(){}
        };

        class Vertex{
        public:
            Vector3 Position;
            Vector3 Color;

            Vertex(Vector3 position, Vector3 color): Position(position), Color(color){}
            
            // This is mainly generated just for the global variables to work.
            Vertex(){}
        };

        class Buffer_Class{
        public:
            VkBuffer Buffer;
            VkDeviceMemory Memory;
            VkDeviceSize Size;

            Buffer_Class(unsigned int size, VkBufferUsageFlagBits type, VkBufferUsageFlagBits flags);

            int Find_Memory_Type(unsigned int typeFilter, VkMemoryPropertyFlags properties);

            bool Set_Data(void* data, unsigned int size, unsigned int offset = 0);

            void Copy_Buffer_To(Buffer_Class& buffer, VkDeviceSize spurce_offset = 0, VkDeviceSize destination_offset = 0);

        };

        // Searches for all graphical devices, uses the first by default.
        extern void Init_Graphical_Devices();

        // Gets all the available surface formats.
        extern void Init_Surface_Formats();

        // Gets all the available present modes.
        extern void Init_Present_Modes();

        // Creates the render pass.
        extern void Init_Render_Pass();

        // creates the pipeline.
        extern void Init_Pipeline();

        // create frame buffer.
        extern void Init_Framebuffers();

        // Creates the Command pool.
        extern void Init_Command_Pool();

        // Creates the command list.
        extern void Init_Command_List();

        // Creates the vertices.
        extern void Init_Vertices();

        // crreates sync objects.
        extern void Init_Sync_Objects();

        // Populates the command buffer with instructions.
        extern void Populate_Command_Buffer();

        // Sends the populated command buffer to gpu.
        extern void Send_Command_Buffer();

        // aquires the next image
        extern int Acquire_Next_Image();

        extern void Present(unsigned int* Image_Index);

        extern void Init_Buffer_Image();

        extern void Update_Frame();

        extern void Render_Frame();

        // Setups the VkSurfaceKHR
        extern void Init(); 
    }

}

#endif