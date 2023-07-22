#include <Elements/Element.h>
#include "Renderer.h"

#include "Vulkan_API.h"

#include <fstream>
#include <thread>


namespace GGUI{
    namespace VULKAN{
        inline Window_Handle            Handle;
        inline VkSurfaceKHR             Surface;
        inline VkInstance               Instance;
        inline Graphical_Device         Selected_Device;
        inline VkSurfaceFormatKHR       Selected_Surface_Format;
        inline VkPresentModeKHR         Selected_Present_Mode;
        inline VkRenderPass             Render_Pass;
        inline Swap_Chain               Swapchain;
        inline VkPipeline               Pipeline;
        inline VkPipelineLayout         Pipeline_Layout;
        inline Shader                   Vertex_Shader;
        inline Shader                   Fragment_Shader;
        inline VkCommandPool            Command_Pool;
        inline VkDebugUtilsMessengerEXT Debug_Messenger;
        inline VkSemaphore              Image_Available_Semaphore;
        inline VkSemaphore              Rendering_Finished_Semaphore;
        inline VkFence                  Frame_Fence;
        inline VkImage                  Frame_Image;
        inline VkDeviceMemory           Frame_Image_Memory;
        inline Buffer_Class             Staging_Buffer;
        inline VkImageView              Frame_Image_View;
        inline VkSampler                Frame_Image_Sampler;
        inline VkDescriptorSetLayout    Descriptor_Set_Layout;

        inline std::atomic_bool         Pause_Vulkan = false;

        inline std::vector<Graphical_Device> Graphical_Devices;
        inline std::vector<VkSurfaceFormatKHR> Surface_Formats;
        inline std::vector<VkPresentModeKHR> Present_Modes;
        inline std::vector<VkCommandBuffer> Command_Buffers;
        inline std::vector<Vertex> Vertices;
        inline const std::vector<const char*> Validation_Layers = {"VK_LAYER_KHRONOS_validation"};
        inline std::vector<RGB> Buffer;

        inline unsigned int Default_Width = 1000;
        inline unsigned int Default_Height = 1000;

        inline bool Use_Debug = true;

        
        #if defined(_WIN32)

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

        void Window_Handle::Window_Events(){
            // msg structure
            MSG msg = {};

            BOOL bRet = 0;

            UpdateWindow(Handle);

            for (int i = 0; (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0 && i < 100; i++){ 
                if (bRet == -1){
                    // handle the error and possibly exit
                }
                else{
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
                } 
            } 

        }

        #else

        Window_Handle::Window_Handle(std::string title, unsigned int width, unsigned int height){
        }

        void Window_Handle::Window_Events(){
        }

        #endif

        Graphical_Device::Graphical_Device(VkPhysicalDevice device){
            Physical_Device = device;

            uint32_t queue_family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, NULL);

            Queue_Families.resize(queue_family_count);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, Queue_Families.data());

            // Now find the index from the queue
            for (size_t i = 0; i < Queue_Families.size(); i++){
                if (Queue_Families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
                    Queue_Index = i;
                    break;
                }
            }

            Get_Physical_Device_Capabilities();
        }

        void Graphical_Device::Get_Physical_Device_Capabilities(){

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Physical_Device, Surface, &Surface_Capabilities);

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
            swapchain_info.imageExtent = Selected_Device.Surface_Capabilities.maxImageExtent;
            swapchain_info.imageFormat = Selected_Surface_Format.format;
            swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            swapchain_info.minImageCount = Image_Count;
            //swapchain_info.pQueueFamilyIndices = (unsigned int*)&Selected_Device.Queue_Index;
            swapchain_info.presentMode = Selected_Present_Mode;
            swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            //swapchain_info.queueFamilyIndexCount = 1;
            swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchain_info.surface = Surface;

            if (vkCreateSwapchainKHR(Selected_Device.Device, &swapchain_info, nullptr, &Swapchain) != VK_SUCCESS){
                std::cout << "Failed to create swapchain." << std::endl;
            }

            unsigned int New_Image_Count = 0;

            if (vkGetSwapchainImagesKHR(Selected_Device.Device, Swapchain, &New_Image_Count, nullptr) != VK_SUCCESS){
                std::cout << "Failed to get swapchain images." << std::endl;
            }

            VkImage* swapchain_images = new VkImage[New_Image_Count];

            if (vkGetSwapchainImagesKHR(Selected_Device.Device, Swapchain, &New_Image_Count, swapchain_images) != VK_SUCCESS){
                std::cout << "Failed to get swapchain images." << std::endl;
            }

            swapchain_image_views.resize(Image_Count);

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

                // VkFramebufferCreateInfo framebuffer_info = {};
                // framebuffer_info.attachmentCount = 1;
                // framebuffer_info.layers = 1;
                // framebuffer_info.pAttachments = &swapchain_image_views[i];
                // framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                // framebuffer_info.width = Selected_Device.Surface_Capabilities.maxImageExtent.width;
                // framebuffer_info.height = Selected_Device.Surface_Capabilities.maxImageExtent.height;
                

                // if (vkCreateFramebuffer(Selected_Device.Device, &framebuffer_info, NULL, &Framebuffers[i]) != VK_SUCCESS){
                //     std::cout << "Failed to create framebuffer." << std::endl;
                // }

            }

        }

        Shader::Shader(std::string path, VkShaderStageFlagBits type){
            std::ifstream file(path, std::ios::ate | std::ios::binary);

            if (!file.is_open()){
                std::cout << "Failed to open shader file: '" + path + "'" << std::endl;
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

        Buffer_Class::Buffer_Class(unsigned int size, VkBufferUsageFlagBits type, VkBufferUsageFlagBits flags){

            VkBufferCreateInfo buffer_info = {};
            buffer_info.size = size;
            buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_info.usage = type;
            buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(Selected_Device.Device, &buffer_info, NULL, &Buffer) != VK_SUCCESS){
                std::cout << "Failed to create buffer." << std::endl;
            }

            VkMemoryRequirements mem_requirements;
            vkGetBufferMemoryRequirements(Selected_Device.Device, Buffer, &mem_requirements);

            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.allocationSize = mem_requirements.size;
            alloc_info.memoryTypeIndex = Find_Memory_Type(mem_requirements.memoryTypeBits, flags);
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

            if (vkAllocateMemory(Selected_Device.Device, &alloc_info, NULL, &Memory) != VK_SUCCESS){
                std::cout << "Failed to allocate memory." << std::endl;
            }

            vkBindBufferMemory(Selected_Device.Device, Buffer, Memory, 0);
        }

        int Buffer_Class::Find_Memory_Type(unsigned int typeFilter, VkMemoryPropertyFlags properties){

            VkPhysicalDeviceMemoryProperties mem_properties;
            vkGetPhysicalDeviceMemoryProperties(Selected_Device.Physical_Device, &mem_properties);

            for (unsigned int i = 0; i < mem_properties.memoryTypeCount; i++){
                if ((typeFilter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties){
                    return i;
                }
            }

            return -1;

        }

        bool Buffer_Class::Set_Data(void* data, unsigned int size, unsigned int offset){
            void* memory;
            if (vkMapMemory(Selected_Device.Device, Memory, offset, size, 0, &memory) != VK_SUCCESS){
                std::cout << "Failed to map memory." << std::endl;
                return false;
            }
            memcpy(memory, data, size);
            vkUnmapMemory(Selected_Device.Device, Memory);

            return true;
        }

        void Buffer_Class::Copy_Buffer_To(Buffer_Class& buffer, VkDeviceSize spurce_offset, VkDeviceSize destination_offset){

            VkBufferCopy info = {};
            info.dstOffset = destination_offset;
            info.size = buffer.Size;
            info.srcOffset = spurce_offset;

            vkCmdCopyBuffer(Command_Buffers[0], Buffer, buffer.Buffer, 1, &info);
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

            Selected_Device = Graphical_Devices[0];

            VkDeviceQueueCreateInfo queue_info = {};
            queue_info.queueCount = 1;
            queue_info.queueFamilyIndex = Selected_Device.Queue_Index;
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.pQueuePriorities = new float{1.0f};

            VkDeviceCreateInfo device_features = {};
            device_features.pQueueCreateInfos = &queue_info;
            device_features.queueCreateInfoCount = 1;
            device_features.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            device_features.enabledExtensionCount = 1;
            device_features.ppEnabledExtensionNames = new const char*{"VK_KHR_swapchain"};
            device_features.enabledLayerCount = 1;
            device_features.ppEnabledLayerNames = new const char*{"VK_LAYER_KHRONOS_validation"};

            if (vkCreateDevice(Selected_Device.Physical_Device, &device_features, NULL, &Selected_Device.Device) != VK_SUCCESS){
                std::cout << "Failed to create device." << std::endl;
            }

            vkGetDeviceQueue(Selected_Device.Device, Selected_Device.Queue_Index, 0, &Selected_Device.Graphics_Queue);



        }

        // Gets all the available surface formats.
        void Init_Surface_Formats(){
            unsigned int Surface_Format_Count = 0;

            if (vkGetPhysicalDeviceSurfaceFormatsKHR(Selected_Device.Physical_Device, Surface, &Surface_Format_Count, 0) != VK_SUCCESS){
                std::cout << "Failed to get surface formats." << std::endl;
            }

            Surface_Formats.resize(Surface_Format_Count);

            if (vkGetPhysicalDeviceSurfaceFormatsKHR(Selected_Device.Physical_Device, Surface, &Surface_Format_Count, Surface_Formats.data()) != VK_SUCCESS){
                std::cout << "Failed to get surface formats." << std::endl;
            }

            // Set default surface format to RGBA 8-bit RGBA.
            for (unsigned int i = 0; i < Surface_Format_Count; i++){
                if (Surface_Formats[i].format == VK_FORMAT_B8G8R8A8_SRGB){
                    Selected_Surface_Format = Surface_Formats[i];
                    break;
                }
            }
        }

        // Gets all the available present modes.
        void Init_Present_Modes(){
            unsigned int Present_Mode_Count = 0;

            if (vkGetPhysicalDeviceSurfacePresentModesKHR(Selected_Device.Physical_Device, Surface, &Present_Mode_Count, 0) != VK_SUCCESS){
                std::cout << "Failed to get present modes." << std::endl;
            }

            Present_Modes.resize(Present_Mode_Count);

            if (vkGetPhysicalDeviceSurfacePresentModesKHR(Selected_Device.Physical_Device, Surface, &Present_Mode_Count, Present_Modes.data()) != VK_SUCCESS){
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

            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT;

            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &color_attachment_ref;

            // subpass depedency
            VkSubpassDependency dependency = {};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo render_pass_info = {};
            render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            render_pass_info.attachmentCount = 1;
            render_pass_info.pAttachments = &color_attachment;
            render_pass_info.subpassCount = 1;
            render_pass_info.pSubpasses = &subpass;
            render_pass_info.pDependencies = &dependency;
            render_pass_info.dependencyCount = 1;

            if (vkCreateRenderPass(Selected_Device.Device, &render_pass_info, NULL, &Render_Pass) != VK_SUCCESS){
                std::cout << "Failed to create render pass." << std::endl;
            }
        }

        // Creates the graphics pipeline.
        void Init_Pipeline(){
            VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
            vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vert_shader_stage_info.module = Vertex_Shader.Module;
            vert_shader_stage_info.pName = "main";

            VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
            frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            frag_shader_stage_info.module = Fragment_Shader.Module;
            frag_shader_stage_info.pName = "main";

            VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

            VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
            vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_info.vertexBindingDescriptionCount = 0;
            vertex_input_info.vertexAttributeDescriptionCount = 0;

            VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
            input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            input_assembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)Selected_Device.Surface_Capabilities.maxImageExtent.width;
            viewport.height = (float)Selected_Device.Surface_Capabilities.maxImageExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor = {};
            scissor.offset = {0, 0};
            scissor.extent = Selected_Device.Surface_Capabilities.maxImageExtent;

            VkPipelineViewportStateCreateInfo viewport_state = {};
            viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_state.viewportCount = 1;
            viewport_state.pViewports = &viewport;
            viewport_state.scissorCount = 1;
            viewport_state.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState color_blend_attachment = {};
            color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment.blendEnable = VK_FALSE;

            VkPipelineColorBlendStateCreateInfo color_blending = {};
            color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blending.logicOpEnable = VK_FALSE;
            color_blending.logicOp = VK_LOGIC_OP_COPY;
            color_blending.attachmentCount = 1;
            color_blending.pAttachments = &color_blend_attachment;
            color_blending.blendConstants[0] = 0.0f;
            color_blending.blendConstants[1] = 0.0f;
            color_blending.blendConstants[2] = 0.0f;
            color_blending.blendConstants[3] = 0.0f;

            VkPipelineLayoutCreateInfo pipeline_layout_info = {};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.setLayoutCount = 1;
            pipeline_layout_info.pSetLayouts = &Descriptor_Set_Layout;
            pipeline_layout_info.pushConstantRangeCount = 0;

            if (vkCreatePipelineLayout(Selected_Device.Device, &pipeline_layout_info, NULL, &Pipeline_Layout) != VK_SUCCESS){
                std::cout << "Failed to create pipeline layout." << std::endl;
            }

            Init_Render_Pass();

            VkGraphicsPipelineCreateInfo pipeline_info = {};
            pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipeline_info.stageCount = 2;
            pipeline_info.pStages = shader_stages;
            pipeline_info.pVertexInputState = &vertex_input_info;
            pipeline_info.pInputAssemblyState = &input_assembly;
            pipeline_info.pViewportState = &viewport_state;
            pipeline_info.pRasterizationState = &rasterizer;
            pipeline_info.pMultisampleState = &multisampling;
            pipeline_info.pDepthStencilState = NULL;
            pipeline_info.pColorBlendState = &color_blending;
            pipeline_info.pDynamicState = NULL;
            pipeline_info.layout = Pipeline_Layout;
            pipeline_info.renderPass = Render_Pass;
            pipeline_info.subpass = 0;
            pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
            pipeline_info.basePipelineIndex = -1;

            if (vkCreateGraphicsPipelines(Selected_Device.Device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &Pipeline) != VK_SUCCESS){
                std::cout << "Failed to create graphics pipeline." << std::endl;
            }

            vkDestroyShaderModule(Selected_Device.Device, Vertex_Shader.Module, NULL);
            vkDestroyShaderModule(Selected_Device.Device, Fragment_Shader.Module, NULL);
        }

        // Creates the frame buffer.
        void Init_Framebuffers(){

            for (size_t i = 0; i < Swapchain.swapchain_image_views.size(); i++){
                VkImageView attachments[] = {
                    Swapchain.swapchain_image_views[i]
                };

                VkFramebufferCreateInfo framebuffer_info = {};
                framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebuffer_info.renderPass = Render_Pass;
                framebuffer_info.attachmentCount = 1;
                framebuffer_info.pAttachments = attachments;
                framebuffer_info.width = Selected_Device.Surface_Capabilities.maxImageExtent.width;
                framebuffer_info.height = Selected_Device.Surface_Capabilities.maxImageExtent.height;
                framebuffer_info.layers = 1;

                if (vkCreateFramebuffer(Selected_Device.Device, &framebuffer_info, NULL, &Swapchain.Framebuffers[i]) != VK_SUCCESS){
                    std::cout << "Failed to create framebuffer." << std::endl;
                }
            }

        }

        // Creates the Command pool.
        void Init_Command_Pool(){

            VkCommandPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.queueFamilyIndex = Selected_Device.Queue_Index;
            pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

            if (vkCreateCommandPool(Selected_Device.Device, &pool_info, NULL, &Command_Pool) != VK_SUCCESS){
                std::cout << "Failed to create command pool." << std::endl;
            }
        }

        // Creates the Command List.
        void Init_Command_List(){

            Command_Buffers.resize(Swapchain.Framebuffers.size());

            for (size_t i = 0; i < Command_Buffers.size(); i++){

                VkCommandBufferAllocateInfo info = {};
                info.commandBufferCount = 1;
                info.commandPool = Command_Pool;
                info.level = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

                vkAllocateCommandBuffers(Selected_Device.Device, &info, &Command_Buffers[i]);
            }
        }

        // Creates the vertices.
        void Init_Vertices(){
            Vertices = {
                {{ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
            };

            Buffer_Class source = Buffer_Class((unsigned int)(sizeof(Vertex) * Vertices.size()), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT));
        
            if (source.Buffer == VK_NULL_HANDLE){
                std::cout << "Failed to create source vertex buffer." << std::endl;
            }

            if (source.Set_Data(Vertices.data(), Vertices.size()) != VK_SUCCESS){
                std::cout << "Failed to set vertex buffer data." << std::endl;
            }

            Buffer_Class destination = Buffer_Class((unsigned int)(sizeof(Vertex) * Vertices.size()), (VkBufferUsageFlagBits)(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

            if (destination.Buffer == VK_NULL_HANDLE){
                std::cout << "Failed to create destination vertex buffer." << std::endl;
            }

            source.Copy_Buffer_To(destination);
        }

        void Populate_Command_Buffer(){

            for (size_t i = 0; i < Command_Buffers.size(); i++){

                VkCommandBufferBeginInfo begin_info = {};
                begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

                vkBeginCommandBuffer(Command_Buffers[i], &begin_info);

                VkRenderPassBeginInfo render_pass_info = {};
                render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                render_pass_info.renderPass = Render_Pass;
                render_pass_info.framebuffer = Swapchain.Framebuffers[i];
                render_pass_info.renderArea.offset = {0, 0};

                render_pass_info.renderArea.extent = Selected_Device.Surface_Capabilities.maxImageExtent;

                VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
                render_pass_info.clearValueCount = 1;

                render_pass_info.pClearValues = &clear_color;

                vkCmdBeginRenderPass(Command_Buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdBindPipeline(Command_Buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, Pipeline);

                // create the viewport
                VkViewport viewport = {};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)Selected_Device.Surface_Capabilities.maxImageExtent.width;
                viewport.height = (float)Selected_Device.Surface_Capabilities.maxImageExtent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                // create the scissor
                VkRect2D scissor = {};
                scissor.offset = {0, 0};
                scissor.extent = Selected_Device.Surface_Capabilities.maxImageExtent;

                vkCmdSetViewport(Command_Buffers[i], 0, 1, &viewport);

                vkCmdSetScissor(Command_Buffers[i], 0, 1, &scissor);

                vkCmdDraw(Command_Buffers[i], 6, 1, 0, 0);

                vkCmdEndRenderPass(Command_Buffers[i]);

                if (vkEndCommandBuffer(Command_Buffers[i]) != VK_SUCCESS){
                    std::cout << "Failed to end command buffer." << std::endl;
                }

            }

        }

        void Sync(){

            // vaits for fence
            vkWaitForFences(Selected_Device.Device, 1, &Frame_Fence, VK_TRUE, UINT64_MAX);
        
            // resets the fence
            vkResetFences(Selected_Device.Device, 1, &Frame_Fence);
            
        }

        void Init_Sync_Objects(){

            VkSemaphoreCreateInfo semaphore_info = {};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fence_info = {};
            fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            if (vkCreateSemaphore(Selected_Device.Device, &semaphore_info, NULL, &Image_Available_Semaphore) != VK_SUCCESS ||
                vkCreateSemaphore(Selected_Device.Device, &semaphore_info, NULL, &Rendering_Finished_Semaphore) != VK_SUCCESS ||
                vkCreateFence(Selected_Device.Device, &fence_info, NULL, &Frame_Fence) != VK_SUCCESS){
                std::cout << "Failed to create sync objects." << std::endl;
            }

        }

        void Send_Command_Buffer(){

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &Command_Buffers[0];

            VkSemaphore wait_semaphores[] = {Image_Available_Semaphore};
            VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = wait_semaphores;
            submit_info.pWaitDstStageMask = wait_stages;

            VkSemaphore signal_semaphores[] = {Rendering_Finished_Semaphore};

            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;

            if (vkQueueSubmit(Selected_Device.Graphics_Queue, 1, &submit_info, Frame_Fence) != VK_SUCCESS){
                std::cout << "Failed to submit command buffer." << std::endl;
            }

        }

        int Acquire_Next_Image(){

            unsigned int Result = 0;
            vkAcquireNextImageKHR(Selected_Device.Device, Swapchain.Swapchain, UINT64_MAX, Image_Available_Semaphore, VK_NULL_HANDLE, &Result);

            return Result;

        }

        void Present(unsigned int* Image_Index){

            VkPresentInfoKHR present_info = {};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = &Rendering_Finished_Semaphore;

            VkSwapchainKHR swapchains[] = {Swapchain.Swapchain};

            present_info.swapchainCount = 1;
            present_info.pSwapchains = swapchains;

            present_info.pImageIndices = Image_Index;

            vkQueuePresentKHR(Selected_Device.Graphics_Queue, &present_info);


        }

        void Init_Buffer_Image(){

            unsigned int Image_Size = Selected_Device.Surface_Capabilities.maxImageExtent.width * Selected_Device.Surface_Capabilities.maxImageExtent.height * 4;

            // create the staging buffer
            Staging_Buffer = Buffer_Class(
                Image_Size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                (VkBufferUsageFlagBits)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            );

            Buffer.resize(Image_Size);

            VkImageCreateInfo image_info = {};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.extent.width = Selected_Device.Surface_Capabilities.maxImageExtent.width;   
            image_info.extent.height = Selected_Device.Surface_Capabilities.maxImageExtent.height;
            image_info.extent.depth = 1;
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.format = VK_FORMAT_R8G8B8_SRGB;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateImage(Selected_Device.Device, &image_info, NULL, &Frame_Image) != VK_SUCCESS){
                std::cout << "Failed to create image." << std::endl;
            }

            VkMemoryRequirements mem_requirements = {};
            vkGetImageMemoryRequirements(Selected_Device.Device, Frame_Image, &mem_requirements);

            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_requirements.size;
            alloc_info.memoryTypeIndex = Staging_Buffer.Find_Memory_Type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(Selected_Device.Device, &alloc_info, NULL, &Frame_Image_Memory) != VK_SUCCESS){
                std::cout << "Failed to allocate image memory." << std::endl;
            }

            vkBindImageMemory(Selected_Device.Device, Frame_Image, Frame_Image_Memory, 0);
        }

        VkCommandBuffer Begin_Single_Time_Commands(){

            VkCommandBufferAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandPool = Command_Pool;
            alloc_info.commandBufferCount = 1;

            VkCommandBuffer command_buffer;
            vkAllocateCommandBuffers(Selected_Device.Device, &alloc_info, &command_buffer);

            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(command_buffer, &begin_info);

            return command_buffer;

        }

        void End_Single_Time_Commands(VkCommandBuffer Command_Buffer){

            vkEndCommandBuffer(Command_Buffer);

            VkSubmitInfo submit_info = {};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &Command_Buffer;

            vkQueueSubmit(Selected_Device.Graphics_Queue, 1, &submit_info, VK_NULL_HANDLE);
            vkQueueWaitIdle(Selected_Device.Graphics_Queue);

            vkFreeCommandBuffers(Selected_Device.Device, Command_Pool, 1, &Command_Buffer);

        }

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
            VkCommandBuffer commandBuffer = Begin_Single_Time_Commands();

            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;

            VkPipelineStageFlags sourceStage;
            VkPipelineStageFlags destinationStage;

            if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
                barrier.srcAccessMask = 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

                sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            } else {
                throw std::invalid_argument("unsupported layout transition!");
            }

            vkCmdPipelineBarrier(
                commandBuffer,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
            );

            End_Single_Time_Commands(commandBuffer);
        }

        void Copy_Buffer_To_Image(VkBuffer Buffer, VkImage Image, uint32_t Width, uint32_t Height){

            VkCommandBuffer command_buffer = Begin_Single_Time_Commands();

            VkBufferImageCopy region = {};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;

            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;

            region.imageOffset = {0, 0, 0};
            region.imageExtent = {
                Width,
                Height,
                1
            };

            vkCmdCopyBufferToImage(command_buffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

            End_Single_Time_Commands(command_buffer);
        }

        void Init_Image_View(){

            VkImageViewCreateInfo view_info = {};
            view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image = Frame_Image;
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = VK_FORMAT_R8G8B8_SRGB;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel = 0;
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(Selected_Device.Device, &view_info, NULL, &Frame_Image_View) != VK_SUCCESS){
                std::cout << "Failed to create image view." << std::endl;
            }

        }

        void Init_Sampler(){

            VkSamplerCreateInfo sampler_info = {};
            sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            sampler_info.magFilter = VK_FILTER_NEAREST;
            sampler_info.minFilter = VK_FILTER_NEAREST;
            sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            sampler_info.anisotropyEnable = VK_TRUE;
            sampler_info.maxAnisotropy = 16;
            sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            sampler_info.unnormalizedCoordinates = VK_FALSE;
            sampler_info.compareEnable = VK_FALSE;
            sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
            sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

            if (vkCreateSampler(Selected_Device.Device, &sampler_info, NULL, &Frame_Image_Sampler) != VK_SUCCESS){
                std::cout << "Failed to create texture sampler." << std::endl;
            }

        }

        void Init_Descriptor_Set_Layout(){
                
            VkDescriptorSetLayoutBinding sampler_layout_binding = {};
            sampler_layout_binding.binding = 0;
            sampler_layout_binding.descriptorCount = 1;
            sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            sampler_layout_binding.pImmutableSamplers = NULL;
            sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutCreateInfo layout_info = {};
            layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layout_info.bindingCount = 1;
            layout_info.pBindings = &sampler_layout_binding;

            if (vkCreateDescriptorSetLayout(Selected_Device.Device, &layout_info, NULL, &Descriptor_Set_Layout) != VK_SUCCESS){
                std::cout << "Failed to create descriptor set layout." << std::endl;
            }

        }

        void Flush_Buffer_To_Image(){
            transitionImageLayout(
                Frame_Image, 
                VK_FORMAT_R8G8B8_SRGB, 
                VK_IMAGE_LAYOUT_UNDEFINED, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            );

            // Move Buffer to Staging_Buffer.Buffer.
            Staging_Buffer.Set_Data((void*)Buffer.data(), Buffer.size() * sizeof(RGB));

            Copy_Buffer_To_Image(
                Staging_Buffer.Buffer, 
                Frame_Image, 
                static_cast<uint32_t>(Selected_Device.Surface_Capabilities.maxImageExtent.width), 
                static_cast<uint32_t>(Selected_Device.Surface_Capabilities.maxImageExtent.height)
            );

            transitionImageLayout(
                Frame_Image, 
                VK_FORMAT_R8G8B8_SRGB, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );
        }

        // Where we construct the texture from the Renderer.h output.
        void Update_Frame(){
            for (int i = 0; i < GGUI::Abstract_Frame_Buffer.size(); i++){
                Buffer[i] = GGUI::Abstract_Frame_Buffer[i].Background;
            }

            // Send the image to the frag shader.
            Flush_Buffer_To_Image();

            Render_Frame();
        }

        // Render the frame
        void Render_Frame(){
            Sync();

            unsigned int Image_Index = Acquire_Next_Image();

            // reset command buffer
            vkResetCommandBuffer(Command_Buffers[Image_Index], 0);

            // populate the command buffer
            Populate_Command_Buffer();

            // send the command buffer
            Send_Command_Buffer();

            Present(&Image_Index);
        }

        bool Check_Validation_Layer_Support() {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            return false;
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL Debug_Callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        ) {

            std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
            } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        void Init_Debug_Messenger(){
            if (!Use_Debug) return;

            VkDebugUtilsMessengerCreateInfoEXT create_info = {};
            create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            create_info.pfnUserCallback = Debug_Callback;

            if (CreateDebugUtilsMessengerEXT(Instance, &create_info, NULL, &Debug_Messenger) != VK_SUCCESS){
                std::cout << "Failed to set up debug messenger." << std::endl;
            }
        }

        // Setups the VkSurfaceKHR
        void Init(){
            Handle = Window_Handle("", Default_Width, Default_Height);

            const char* extension[] = {
                VK_KHR_SURFACE_EXTENSION_NAME
                ,VK_EXT_DEBUG_UTILS_EXTENSION_NAME
                //VK_KHR_SWAPCHAIN_EXTENSION_NAME
                #if defined(_WIN32)
                ,VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                #endif
            };

            VkApplicationInfo Application_Info = {};
            Application_Info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            Application_Info.pApplicationName = "Vulkan";
            Application_Info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            Application_Info.pEngineName = "No Engine";
            Application_Info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            Application_Info.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo Instance_Info = {};
            Instance_Info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            Instance_Info.enabledExtensionCount = sizeof(extension) / sizeof(extension[0]);
            Instance_Info.ppEnabledExtensionNames = extension;
            Instance_Info.pApplicationInfo = &Application_Info;

            if (Use_Debug){
                Instance_Info.enabledLayerCount = static_cast<uint32_t>(Validation_Layers.size());
                Instance_Info.ppEnabledLayerNames = Validation_Layers.data();
            }

            vkCreateInstance(&Instance_Info, 0, &Instance);
            Init_Debug_Messenger();

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
            
            Swapchain = Swap_Chain(Default_Width, Default_Height);

            Vertex_Shader = Shader("Shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);

            Fragment_Shader = Shader("Shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

            Init_Pipeline();

            Init_Framebuffers();

            Init_Command_Pool();
            Init_Command_List();

            Init_Sync_Objects();

            std::thread Window_Events_Thread([&](){
                Handle.Window_Events();
            });

            Update_Frame();

            return;
            //Init_Vertices();
        }

    }
}