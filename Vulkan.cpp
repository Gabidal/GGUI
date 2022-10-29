#include "Vulkan.h"

#include <fstream>

namespace GGUI{
    Window_Handle::Window_Handle(std::string title, unsigned int width, unsigned int height){
        
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

    Graphical_Device::Graphical_Device(VkPhysicalDevice device){
        Device = device;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

        Queue_Families.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, Queue_Families.data());
    }

    Swap_Chain::Swap_Chain(unsigned int width, unsigned int height){
        Width = width;
        Height = height;

        unsigned int Image_Count = 2;

        VkSwapchainCreateInfoKHR swapchain_info = {};
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageColorSpace = Selected_Surface_Format.colorSpace;
        swapchain_info.imageExtent.height = Width;
        swapchain_info.imageExtent.width = Height;
        swapchain_info.imageFormat = Selected_Surface_Format.format;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchain_info.minImageCount = Image_Count;
        swapchain_info.pQueueFamilyIndices = Selected_Device.Queue_Families.data();
        swapchain_info.presentMode = Selected_Present_Mode;
        swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_info.queueFamilyIndexCount = Selected_Device.Queue_Families.size();
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = Surface;

        if (vkCreateSwapchainKHR(Selected_Device.Device, &swapchain_info, NULL, &Swapchain) != VK_SUCCESS){
            std::cout << "Failed to create swapchain." << std::endl;
        }

        VkImage swapchain_images[(const unsigned int)Image_Count];
        if (vkGetSwapchainImagesKHR(Selected_Device.Device, Swapchain, &Image_Count, swapchain_images) != VK_SUCCESS){
            std::cout << "Failed to get swapchain images." << std::endl;
        }

        VkImageView swapchain_image_views[(const unsigned int)Image_Count];

        Framebuffers.resize(Image_Count);

        for (unsigned int i = 0; i < Image_Count; i++){

            VkImageViewCreateInfo image_view_info = {};
            image_view_info.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
            image_view_info.format = Selected_Surface_Format.format;
            image_view_info.image = swapchain_images[i];
            image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_info.subresourceRange.baseArrayLayer = 0;
            image_view_info.subresourceRange.baseMipLevel = 0;
            image_view_info.subresourceRange.layerCount = 1;
            image_view_info.subresourceRange.levelCount = 1;
            image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;

            if (vkCreateImageView(Selected_Device.Device, &image_view_info, NULL, &swapchain_image_views[i]) != VK_SUCCESS){
                std::cout << "Failed to create image view." << std::endl;
            }

            VkFramebufferCreateInfo framebuffer_info = {};
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.layers = 1;
            framebuffer_info.pAttachments = &swapchain_image_views[i];
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.width = Width;
            framebuffer_info.height = Height;

            if (vkCreateFramebuffer(Selected_Device.Device, &framebuffer_info, NULL, &Framebuffers[i]) != VK_SUCCESS){
                std::cout << "Failed to create framebuffer." << std::endl;
            }

        }

    }

    Shader::Shader(std::string path, Shader_Type type){
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()){
            std::cout << "Failed to open shader file." << std::endl;
        }

        size_t file_size = (size_t)file.tellg();
        std::vector<char> buffer(file_size);

        file.seekg(0);
        file.read(buffer.data(), file_size);

        file.close();

        VkShaderModuleCreateInfo shader_info = {};
        shader_info.codeSize = file_size;
        shader_info.pCode = (const uint32_t*)buffer.data();
        shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

        if (vkCreateShaderModule(Selected_Device.Device, &shader_info, NULL, &Module) != VK_SUCCESS){
            std::cout << "Failed to create shader module." << std::endl;
        }

        Type = type;
    }

        // Searches for all graphical devices, uses the first by default.
    void Init_Graphical_Devices(){

        unsigned int Graphical_Device_Count = 0;

        if (vkEnumeratePhysicalDevices(Instance, &Graphical_Device_Count, 0) != VK_SUCCESS){
            std::cout << "Failed to enumerate physical devices." << std::endl;
        }

        VkPhysicalDevice *physical_devices = new VkPhysicalDevice[Graphical_Device_Count];
        if (vkEnumeratePhysicalDevices(Instance, &Graphical_Device_Count, physical_devices) != VK_SUCCESS){
            std::cout << "Failed to enumerate physical devices." << std::endl;
        }

        for (unsigned int i = 0; i < Graphical_Device_Count; i++){
            
            Graphical_Device Device(physical_devices[i]);

            Graphical_Devices.push_back(Device);

        }
    }

    // Gets all the available surface formats.
    void Init_Surface_Formats(){
        unsigned int Surface_Format_Count = 0;

        if (vkGetPhysicalDeviceSurfaceFormatsKHR(Selected_Device.Device, Surface, &Surface_Format_Count, 0) != VK_SUCCESS){
            std::cout << "Failed to get surface formats." << std::endl;
        }

        Surface_Formats.resize(Surface_Format_Count);

        if (vkGetPhysicalDeviceSurfaceFormatsKHR(Selected_Device.Device, Surface, &Surface_Format_Count, Surface_Formats.data()) != VK_SUCCESS){
            std::cout << "Failed to get surface formats." << std::endl;
        }

        // Set default surface format to RGBA 8-bit RGBA.
        for (unsigned int i = 0; i < Surface_Format_Count; i++){
            if (Surface_Formats[i].format == VK_FORMAT_R8G8B8A8_UNORM){
                Selected_Surface_Format = Surface_Formats[i];
                break;
            }
        }
    }

    // Gets all the available present modes.
    void Init_Present_Modes(){
        unsigned int Present_Mode_Count = 0;

        if (vkGetPhysicalDeviceSurfacePresentModesKHR(Selected_Device.Device, Surface, &Present_Mode_Count, 0) != VK_SUCCESS){
            std::cout << "Failed to get present modes." << std::endl;
        }

        Present_Modes.resize(Present_Mode_Count);

        if (vkGetPhysicalDeviceSurfacePresentModesKHR(Selected_Device.Device, Surface, &Present_Mode_Count, Present_Modes.data()) != VK_SUCCESS){
            std::cout << "Failed to get present modes." << std::endl;
        }

        // Set default present mode to FIFO.
        for (unsigned int i = 0; i < Present_Mode_Count; i++){
            if (Present_Modes[i] == VK_PRESENT_MODE_FIFO_KHR){
                Selected_Present_Mode = Present_Modes[i];
                break;
            }
        }
    }

    // Creates the render pass.
    void Init_Render_Pass(){
        VkAttachmentDescription color_attachment = {};
        color_attachment.format = Selected_Surface_Format.format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;

        VkRenderPassCreateInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = 1;
        render_pass_info.pAttachments = &color_attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;

        if (vkCreateRenderPass(Selected_Device.Device, &render_pass_info, NULL, &Render_Pass) != VK_SUCCESS){
            std::cout << "Failed to create render pass." << std::endl;
        }
    }

    // Setups the VkSurfaceKHR
    void Init(){
        Window_Handle Handle("", Default_Width, Default_Height);

        const char* extension[] = {
            VK_KHR_SURFACE_EXTENSION_NAME
            #if defined(_WIN32)
            ,VK_KHR_WIN32_SURFACE_EXTENSION_NAME
            #endif
        };

        VkInstanceCreateInfo Instance_Info = {};
        Instance_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        Instance_Info.enabledExtensionCount = sizeof(extension) / sizeof(extension[0]);
        Instance_Info.ppEnabledExtensionNames = extension;

        vkCreateInstance(&Instance_Info, 0, &Instance);

        // This is the Windows specific code
        #if defined(_WIN32)
            
        VkWin32SurfaceCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        info.hinstance = GetModuleHandle(NULL);
        info.hwnd = Handle.Handle;

        vkCreateWin32SurfaceKHR(Instance, &info, NULL, &Surface);

        // This is the Linux specific code
        #else

        #endif

        // This is code for all of em.
        Init_Graphical_Devices();
        Init_Surface_Formats();
        Init_Present_Modes();
        Init_Render_Pass();
        
        Swapchain = Swap_Chain(Default_Width, Default_Height);




    }


}