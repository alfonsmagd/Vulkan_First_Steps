#include "VulkanSystemRendererd.h"


namespace VK_Test {
	
	
	HANDLE STD_OutputColor = GetStdHandle(STD_OUTPUT_HANDLE);

	static VKAPI_ATTR VkBool32 VKAPI_CALL 
	debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		//Only change the color about VulkanDebugger Info Layer in Pink. 
		SetConsoleTextAttribute(STD_OutputColor, 13);
		std::cerr << "Validation Layer [Info:]" << pCallbackData->pMessage << std::endl;
		SetConsoleTextAttribute(STD_OutputColor, 15);
		//In this function its posible to filter with messageSeverity. 

		return VK_FALSE;
	}


	bool const
	VulkanSystemRendered_t::chekInstanceExtensionsSupports(VectExtensions* checkExtensions) const
	{
		//Need to get number of extensions to hold and create array with correct size extension.
		uint32_t countExtension = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &countExtension, nullptr);
	

		//Create a vector with VkExtensionProperties 
		std::vector<VkExtensionProperties> extensions(countExtension);
		vkEnumerateInstanceExtensionProperties(nullptr, &countExtension, extensions.data());

		//Check if given extensions are in list of avaliable extensions.
		for (const auto& chek : *checkExtensions) {

			bool hasExtension = false;

			for (const auto& ext : extensions) {

				if (strcmp(ext.extensionName, chek) == 0) {
					hasExtension = true;
					break;
				}//compare extensions for, 
			}
			if (!hasExtension)
				return false;

		}//for checkExtensions

		return true;
	}

	auto
	VulkanSystemRendered_t::getSwapChainDetails(VkPhysicalDevice device)
	{
		SwapChainDetails swapChainDeatails;
		//Get the surface capabilites for the given physical device and given surface. 
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDeatails.surfaceCapabilites);

		//Fortmas. 
		uint32_t countFormat = 0;
		bool     isEmptyFormats = true;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &countFormat, nullptr);

		if (countFormat != 0) {
			swapChainDeatails.formats.resize(countFormat);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &countFormat, &*(swapChainDeatails.formats.begin()));
			isEmptyFormats = swapChainDeatails.formats.empty();
		}
		
		//Presentation
		uint32_t countPresentModes = 0;
		bool     isEmptyPresentModes = true;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &countPresentModes, nullptr);

		if (countPresentModes != 0) {
			swapChainDeatails.presentmode.resize(countPresentModes);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &countPresentModes, &*(swapChainDeatails.presentmode.begin()));
			isEmptyPresentModes = swapChainDeatails.presentmode.empty();
		}


		return std::tuple{swapChainDeatails,isEmptyFormats,isEmptyPresentModes };
	}

	VkSurfaceFormatKHR
	VulkanSystemRendered_t::getBetterSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>& formats)
	{

		if (formats.size() == 1 || formats[0].format == VK_FORMAT_UNDEFINED) {
			std::cout << "[VulkanSystemRendered_t::getBetterSurfaceFormatKHR](Warning)-->Force Choose correct format with succes\n";
			return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
		}


		for (const auto& format : formats) {

			if (format.format == VK_FORMAT_R8G8B8A8_UNORM ||
				format.format == VK_FORMAT_B8G8R8A8_UNORM ||
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {

				std::cout << "[VulkanSystemRendered_t::getBetterSurfaceFormatKHR](INFO)-->Choose correct format with succes\n";
				return format;
			}
		}
		std::cout << "[VulkanSystemRendered_t::getBetterSurfaceFormatKHR](ERROR)-->Return nothing, imposible to chosse better surfaceFormat \n";
		return{};

	}

	VkPresentModeKHR
	VulkanSystemRendered_t::getBetterPresentModeKHR(const std::vector<VkPresentModeKHR>& presentsModes)
	{
		//Look for mailbox presentatio mode.
		for (const auto& presentMode : presentsModes) {

			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentMode;

		}
		//If can´t find it, use FIFO . Vulkan recommend it. 
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D
	VulkanSystemRendered_t::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapability)
	{
		if (surfaceCapability.currentExtent.width != std::numeric_limits<uint32_t>::max()) {

			return surfaceCapability.currentExtent;
		}
		else
		{
			//Get the glfw capabilites and retrieves of the framebuffer of the specified windows and net to set manually 
			int width, height;

			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D manuallyExtent{};
			manuallyExtent.height = static_cast<uint32_t>(height);
			manuallyExtent.width  = static_cast<uint32_t>(width);

			//Surface also defines max and min, so make sure within boundaries by clamping value
			manuallyExtent.height = std::max(surfaceCapability.minImageExtent.height, std::min(surfaceCapability.maxImageExtent.height, manuallyExtent.height));
			manuallyExtent.width = std::max(surfaceCapability.minImageExtent.width, std::min(surfaceCapability.maxImageExtent.width, manuallyExtent.width));
			
			return manuallyExtent;
		}

	}

	//this function only set up and initialize ExtensionNames Vulkan requiered by GLFW. 
	 VectExtensions
	 VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo() {

		 std::cout << "[VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo](INFO)-->Set up  and cheking extensions names....\n";
		auto instanceExtensions = VectExtensions();
		uint32_t glfwExtensionsCount = 0;

		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		for (size_t i = 0; i < glfwExtensionsCount; ++i) {
			instanceExtensions.push_back(glfwExtensions[i]);
		}

		//Add extension if enablevalidationLayer is available.
		if (enableValidationLayers == true)
			instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		//Chek Instance Extensions is supported. 
		if (!chekInstanceExtensionsSupports(&instanceExtensions)) {
			std::cout <<"[VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo](ERROR)-->Vulkan not have extenions correctly\n";
			return{};
		}

		else {
			std::cout<< "[VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo](INFO)-->Vulkan have correctly extension to work\n";
			return instanceExtensions;
		}
	
	}

	 bool
	 VulkanSystemRendered_t::checkDeviceSuitable(VkPhysicalDevice device) 
	 {
		 
		 //Information about the device itself (ID,Vendedor,type,..)
		 VkPhysicalDeviceProperties deviceProperties;
		 vkGetPhysicalDeviceProperties(device, &deviceProperties);

		 //Information about what the device can do (geoshader,tesering,..ect)
		 VkPhysicalDeviceFeatures deviceFeatures;
		 vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		 indicesQueueFamily = getQueueFamilies(device);

		 //Check if extensions is supported by device. 
		 bool isextensionsSuported = checkDeviceExtensionsSupport(device);

		 auto [swapChainDeatails, isEmptyFormats, isEmptyPresentModes] = getSwapChainDetails(device);

		 return indicesQueueFamily.isValid() && isextensionsSuported && !isEmptyFormats && !isEmptyPresentModes;
	 }

	 bool
	 VulkanSystemRendered_t::checkValidationLayerSupport()
	 {
		 //1.Get all layer available.
		 uint32_t layercount;
		 vkEnumerateInstanceLayerProperties(&layercount, nullptr);

		 std::vector<VkLayerProperties> availableLayers(layercount);
		 vkEnumerateInstanceLayerProperties(&layercount, availableLayers.data());

		 //2.check if availableLayer exits in validationLayers. 
		 for (const char* layerName : validationLayers) {
			 bool layerFound = false;

			 for (const auto& layerProperties : availableLayers) {
				 if (strcmp(layerName, layerProperties.layerName) == 0) {
					 layerFound = true;
					 break;
				 }
			 }
			 if (!layerFound) {
				 std::cout << "[VulkanSystemRendered_t::checkValidationLayerSupport](Warning)-->Not Layer found\n";
				 return false;
			 }
		 }
		 std::cout << "[VulkanSystemRendered_t::checkValidationLayerSupport](INFO)-->Layer found\n";
		 return true;

	 }

	 void
	 VulkanSystemRendered_t::setupDebugMessenger()
	 {
		 if (enableValidationLayers == false)return;

		 //Create debugcreateinfo and fill with diferents features. 
		 VkDebugUtilsMessengerCreateInfoEXT dbugcreateInfo{};
		 populateDebugMessengerCreateInfo(dbugcreateInfo);

		 VkResult result = CreateDebugUtilsMessengerEXT(instance, &dbugcreateInfo, nullptr, &debugMessenger);

		 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createInstance](ERROR)-->Creating Instance FAIL" << std::endl :
			 std::cout << "[VulkanSystemRendered_t::createInstance](INFO)-->Creating Instance SUCCESS" << std::endl;
		
		 
	 }

	 void VulkanSystemRendered_t::createCommandBuffer()
	 {
		 commandBuffers.resize(swapChainFramebuffers.size());

		 VkCommandBufferAllocateInfo cbAllocateInfo{};
		 cbAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		 cbAllocateInfo.commandPool = commandPool;
		 cbAllocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		 cbAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                              // Buffer you submit directly to queue. Cant be called by others buffers 
																					          // VK_COMMAND_BUFFER_LEVEL_SECONDARY : Buffer you cant submit directly to queue, but can called by others buffers using "vkCmdExecuteCommands" when recording commands. 
		 VkResult result = vkAllocateCommandBuffers(mainDevice.logicalDevice, &cbAllocateInfo, commandBuffers.data());

		 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createCommandBuffer](ERROR)-->Failed to create allocate command buffer\n" :
			 std::cout << "[VulkanSystemRendered_t::createCommandBuffer](INFO)-->Allocate Command buffer was be create with Success\n";



	 }

	 bool 
     VulkanSystemRendered_t::checkDeviceExtensionsSupport(VkPhysicalDevice device)
	 {
		 //Need to get number of extensions to hold and create array with correct size extension.
		 uint32_t countDeviceExtensions = 0;
		 vkEnumerateDeviceExtensionProperties(device, nullptr, &countDeviceExtensions, nullptr);

		 if (countDeviceExtensions != 0) {

			 //Create a vector with VkExtensionProperties 
			 std::vector<VkExtensionProperties> extensions(countDeviceExtensions);
			 vkEnumerateDeviceExtensionProperties(device, nullptr, &countDeviceExtensions, &extensions[0]);

			 //Check if given extensions are in list of avaliable extensions.
			 for (const auto& chekExtension : deviceExtensions) {

				 bool hasExtension = false;

				 for (const auto& extension : extensions) {

					 if (strcmp(extension.extensionName, chekExtension) == 0) {
						 hasExtension = true;
						 break;
					 }//compare extensions for, 
				 }
				 if (!hasExtension)
					 return false;

			 }//for checkExtensions
			 return true;
		 }
		 else {
			 std::cout << "[VulkanSystemRendered_t::checkDeviceExtensionsSupport](WARNING)-->Not device extensions found it. \n";
			 return false;
		 }
	 }

	 VkResult
     VulkanSystemRendered_t::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	 {
		 auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		 if (func != nullptr) {
			 return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		 }
		 else {
			 return VK_ERROR_EXTENSION_NOT_PRESENT;
		 }
	 }

	 void
     VulkanSystemRendered_t::createSwapChain()
	 {
		
		 std::cout << "[VulkanSystemRendered_t::createSwapChain](INFO)-->Building a Swapchain....\n";
		 //Get swapchaindetails to pick better options. 
		 auto [swapchainDetails, isemptyFormat, isemptyPresentModes] = getSwapChainDetails(mainDevice.physicalDevice);
		 //1. Choose better surface format. 
		 VkSurfaceFormatKHR bestSurfaceFormat{};

		 if (!isemptyFormat) {

		      bestSurfaceFormat = getBetterSurfaceFormatKHR(swapchainDetails.formats);
		 }
		 else std::cout << "[VulkanSystemRendered_t::createSwapChain](WARNING)-->Empty format SwapChainDeatils.formats are empty probably error produces....\n";
		 //2. Chose better  presentation mode. 
		 VkPresentModeKHR bestPresentMode{};
		 if (!isemptyPresentModes) {
			 
			 bestPresentMode = getBetterPresentModeKHR(swapchainDetails.presentmode);
		 }
		 else std::cout << "[VulkanSystemRendered_t::createSwapChain](WARNING)-->Empty format SwapChainDeatils.presentmode are empty probably error produces....\n";
		 //3. Chose SWAP CHAIN MODE. 
		 VkExtent2D extent2D = chooseSwapExtent(swapchainDetails.surfaceCapabilites);

		 //3.1 To choose how many images are in the swap chain . One more that the minimum to allow triple buffering. 
		 uint32_t imageCount = swapchainDetails.surfaceCapabilites.minImageCount + 1;

		 //3.2 If imageCounter is higher than max, then clamp down to max. 
		 if (imageCount > swapchainDetails.surfaceCapabilites.maxImageCount && 
			 swapchainDetails.surfaceCapabilites.minImageCount ) {

			 imageCount = swapchainDetails.surfaceCapabilites.maxImageCount;
		 }
		 //4. Create Information for Swap chain 
			 VkSwapchainCreateInfoKHR swapChainInfo{};
			 createSwapChainInfo(bestSurfaceFormat, bestPresentMode, extent2D, swapchainDetails, imageCount, &swapChainInfo);

			 VkResult result =   vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapChainInfo, nullptr, &swapchain);
			 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createSwapChain](ERROR)-->Failed to create SwapChain\n" :
									std::cout << "[VulkanSystemRendered_t::createSwapChain](INFO)-->SwapChain  was be create with Success\n";

		  //4.1. Get Volkan componentes to use after. 
			 if (result == VK_SUCCESS) {

				 swapChainImageFormat = bestSurfaceFormat.format;
				 swapChainExtent = extent2D;

			 }

		  //5. Get swap chain images, ( first  count , then values)

			 uint32_t swapChainImagesCount;

			 vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImagesCount, nullptr);
			 std::vector<VkImage> images(swapChainImagesCount);
			 vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImagesCount,&*images.begin());

			 for (const auto& image : images) {
				 //Store Images Handle 
				 SwapChainImage swapChainImage{};
				 swapChainImage.image = image;
				 swapChainImage.imageView = createImageView(image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

				 //Add to swapchainimage in a list.
				 swapChainImages.push_back(swapChainImage);
			}
			
	 }

	 void
	 VulkanSystemRendered_t::createSwapChainInfo(const VkSurfaceFormatKHR& surfaceFormat, const VkPresentModeKHR& presentMode, VkExtent2D extent, const SwapChainDetails& details, const uint32_t imageCount, VkSwapchainCreateInfoKHR* swapChainInfo)
	 {
			 swapChainInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			 swapChainInfo->surface = surface;
			 swapChainInfo->imageArrayLayers = 1;													//Number of layers for each image in chain. 
			 swapChainInfo->imageFormat = surfaceFormat.format;
			 swapChainInfo->imageColorSpace = surfaceFormat.colorSpace;
			 swapChainInfo->presentMode = presentMode;
			 swapChainInfo->imageExtent = extent;
			 swapChainInfo->minImageCount = imageCount;
			 swapChainInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			 swapChainInfo->preTransform = details.surfaceCapabilites.currentTransform;
			 swapChainInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                     //How to handle blending images with externals graphics (e.g other window)
			 swapChainInfo->clipped = VK_TRUE;													   //Wheter to clipp part of image , not view 

			 QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

			 //If graphics and presentation family are diferent, then swapchain must let images be shared betwen families. 
			 if (indices.graphicsFamily != indices.presentationFamily) {

				 //Queues to be  shared 
				 uint32_t queueFamilyIndicesBuild[] = {

					 static_cast<uint32_t>(indices.graphicsFamily),
					 static_cast<uint32_t>(indices.presentationFamily),

				 };

				 swapChainInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;						//Images share handling
				 swapChainInfo->queueFamilyIndexCount = 2;											//Number of queues to shared images betwen 
				 swapChainInfo->pQueueFamilyIndices = queueFamilyIndicesBuild;						//Arrays of queues to  shared between. 

			 }else {
				 swapChainInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				 swapChainInfo->queueFamilyIndexCount = 0;
				 swapChainInfo->pQueueFamilyIndices = nullptr;

			 }
			
			 //If old swapchain has benn destroyed and new swapchain replaces it , then link old one to quiqcly have responsabilities. 
			 swapChainInfo->oldSwapchain = VK_NULL_HANDLE;
	 }

	 VkImageView
	 VulkanSystemRendered_t::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectflags)
	 {
		 std::cout << "[VulkanSystemRendered_t::createImageView](INFO)-->ImageView is Creating........\n";

		 VkImageViewCreateInfo imageViewInfo{};
		 imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		 imageViewInfo.image = image;												//Image to create view for. 
		 imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;							//Type of image ( 1D,2D,CUBE)
		 imageViewInfo.format = format;												//Format of image Data. 
		 imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;				
		 imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		 imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		 imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		 
		 //SobreSource allow the view only a part of an image. 
		 imageViewInfo.subresourceRange.aspectMask = aspectflags;					//Which aspect of image to view
		 imageViewInfo.subresourceRange.baseMipLevel = 0;							//Start base mipmap leve to view from 
		 imageViewInfo.subresourceRange.baseArrayLayer = 0;							//Start array level to view from 
		 imageViewInfo.subresourceRange.layerCount = 1;								//Number of array levels to view
		 imageViewInfo.subresourceRange.levelCount = 1;								//is the number of mipmap levels (starting from baseMipLevel) accessible to the view

		 //Create ImageView and return it 

		 VkImageView imageView{};

		 VkResult result = vkCreateImageView(mainDevice.logicalDevice, &imageViewInfo, nullptr, &imageView);
		
		 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createImageView](ERROR)-->Failed to create ImageView\n" :
			 std::cout << "[VulkanSystemRendered_t::createImageView](INFO)-->ImageView was be create with Success\n";

		 return imageView;
	 }

	 void
	 VulkanSystemRendered_t::createGraphicsPipeLine()
	 {
		 // Read in SPIR-V code of shaders
		 auto vertexShaderCode = readFromFile("Shaders/vert.spv");
		 auto fragmentShaderCode = readFromFile("Shaders/frag.spv");

		 // Create Shader Modules
		 VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
		 VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);

		 // -- SHADER STAGE CREATION INFORMATION --
		 // Vertex Stage creation information
		 VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
		 vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		 vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;				// Shader Stage name
		 vertexShaderCreateInfo.module = vertexShaderModule;						// Shader module to be used by stage
		 vertexShaderCreateInfo.pName = "main";									// Entry point in to shader

		 // Fragment Stage creation information
		 VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
		 fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		 fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;				// Shader Stage name
		 fragmentShaderCreateInfo.module = fragmentShaderModule;						// Shader module to be used by stage
		 fragmentShaderCreateInfo.pName = "main";									// Entry point in to shader

		 // Put shader stage creation info in to array
		 // Graphics Pipeline creation info requires array of shader stage creates
		 VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo };


		 // -- VERTEX INPUT (TODO: Put in vertex descriptions when resources created) --
		 VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		 vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		 vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		 vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;			// List of Vertex Binding Descriptions (data spacing/stride information)
		 vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		 vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;		// List of Vertex Attribute Descriptions (data format and where to bind to/from)


		 // -- INPUT ASSEMBLY --
		 VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		 inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		 inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;		// Primitive type to assemble vertices as
		 inputAssembly.primitiveRestartEnable = VK_FALSE;					// Allow overriding of "strip" topology to start new primitives


		 // -- VIEWPORT & SCISSOR --
		 // Create a viewport info struct
		 VkViewport viewport = {};
		 viewport.x = 0.0f;									// x start coordinate
		 viewport.y = 0.0f;									// y start coordinate
		 viewport.width = (float)swapChainExtent.width;		// width of viewport
		 viewport.height = (float)swapChainExtent.height;	// height of viewport
		 viewport.minDepth = 0.0f;							// min framebuffer depth
		 viewport.maxDepth = 1.0f;							// max framebuffer depth

		 // Create a scissor info struct
		 VkRect2D scissor = {};
		 scissor.offset = { 0,0 };							// Offset to use region from
		 scissor.extent = swapChainExtent;					// Extent to describe region to use, starting at offset

		 VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		 viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		 viewportStateCreateInfo.viewportCount = 1;
		 viewportStateCreateInfo.pViewports = &viewport;
		 viewportStateCreateInfo.scissorCount = 1;
		 viewportStateCreateInfo.pScissors = &scissor;


		 // -- DYNAMIC STATES --
		 // Dynamic states to enable
		 //std::vector<VkDynamicState> dynamicStateEnables;
		 //dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);	// Dynamic Viewport : Can resize in command buffer with vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
		 //dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);	// Dynamic Scissor	: Can resize in command buffer with vkCmdSetScissor(commandbuffer, 0, 1, &scissor);

		 //// Dynamic State creation info
		 //VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		 //dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		 //dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
		 //dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();


		 // -- RASTERIZER --
		 VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		 rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		 rasterizerCreateInfo.depthClampEnable = VK_FALSE;			// Change if fragments beyond near/far planes are clipped (default) or clamped to plane
		 rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;	// Whether to discard data and skip rasterizer. Never creates fragments, only suitable for pipeline without framebuffer output
		 rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;	// How to handle filling points between vertices
		 rasterizerCreateInfo.lineWidth = 1.0f;						// How thick lines should be when drawn
		 rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;		// Which face of a tri to cull
		 rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;	// Winding to determine which side is front
		 rasterizerCreateInfo.depthBiasEnable = VK_FALSE;			// Whether to add depth bias to fragments (good for stopping "shadow acne" in shadow mapping)


		 // -- MULTISAMPLING --
		 VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
		 multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		 multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;					// Enable multisample shading or not
		 multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// Number of samples to use per fragment


		 // -- BLENDING --
		 // Blending decides how to blend a new colour being written to a fragment, with the old value

		 // Blend Attachment State (how blending is handled)
		 VkPipelineColorBlendAttachmentState colourState = {};
		 colourState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT	// Colours to apply blending to
			 | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		 colourState.blendEnable = VK_TRUE;													// Enable blending

		 // Blending uses equation: (srcColorBlendFactor * new colour) colorBlendOp (dstColorBlendFactor * old colour)
		 colourState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		 colourState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		 colourState.colorBlendOp = VK_BLEND_OP_ADD;

		 // Summarised: (VK_BLEND_FACTOR_SRC_ALPHA * new colour) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * old colour)
		 //			   (new colour alpha * new colour) + ((1 - new colour alpha) * old colour)

		 colourState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		 colourState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		 colourState.alphaBlendOp = VK_BLEND_OP_ADD;
		 // Summarised: (1 * new alpha) + (0 * old alpha) = new alpha

		 VkPipelineColorBlendStateCreateInfo colourBlendingCreateInfo = {};
		 colourBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		 colourBlendingCreateInfo.logicOpEnable = VK_FALSE;				// Alternative to calculations is to use logical operations
		 colourBlendingCreateInfo.attachmentCount = 1;
		 colourBlendingCreateInfo.pAttachments = &colourState;


		 // -- PIPELINE LAYOUT (TODO: Apply Future Descriptor Set Layouts) --
		 VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		 pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		 pipelineLayoutCreateInfo.setLayoutCount = 0;
		 pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		 pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		 pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		 // Create Pipeline Layout
		 VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
		 if (result != VK_SUCCESS)
		 {
			 throw std::runtime_error("Failed to create Pipeline Layout!");
		 }


		 // -- DEPTH STENCIL TESTING --
		 // TODO: Set up depth stencil testing


		 // -- GRAPHICS PIPELINE CREATION --
		 VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		 pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		 pipelineCreateInfo.stageCount = 2;									// Number of shader stages
		 pipelineCreateInfo.pStages = shaderStages;							// List of shader stages
		 pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;		// All the fixed function pipeline states
		 pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
		 pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		 pipelineCreateInfo.pDynamicState = nullptr;
		 pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
		 pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
		 pipelineCreateInfo.pColorBlendState = &colourBlendingCreateInfo;
		 pipelineCreateInfo.pDepthStencilState = nullptr;
		 pipelineCreateInfo.pTessellationState = nullptr;
		 pipelineCreateInfo.layout = pipelineLayout;							// Pipeline Layout pipeline should use
		 pipelineCreateInfo.renderPass = renderPass;							// Render pass description the pipeline is compatible with
		 pipelineCreateInfo.subpass = 0;										// Subpass of render pass to use with pipeline

		 // Pipeline Derivatives : Can create multiple pipelines that derive from one another for optimisation
		 pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;	// Existing pipeline to derive from...
		 pipelineCreateInfo.basePipelineIndex = -1;				// or index of pipeline being created to derive from (in case creating multiple at once)

		 // Create Graphics Pipeline
		 
		 result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, nullptr, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline);

		 // Destroy Shader Modules, no longer needed after Pipeline created
		 vkDestroyShaderModule(mainDevice.logicalDevice, fragmentShaderModule, nullptr);
		 vkDestroyShaderModule(mainDevice.logicalDevice, vertexShaderModule, nullptr);
	 }

	 void
	 VulkanSystemRendered_t::createRenderPass()
	 {
		
		 //Color atachment descritpion 
		 VkAttachmentDescription colorAttachment{};
		 colorAttachment.format = swapChainImageFormat;
		 colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;						//number of samples to write for multisampling.
		 colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;					// Describes what to do with atacchamente before rendering. (Perserver in this case,color and depth)
		 colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;				//Describes what to do with atacchment afert rendering (be stored in memory and can be read later)
		 colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;		//Describes what to do with stencil before rendering(aply to stencil data)
		 colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;		//Describes what to do with stencil after rendering (aply to stencil data)

		 //FrameBufferData will be stored as an image, Vulkan are represented by VkImage objects with a certain pixel format, however the layout of the pixels in memory can change based
		 //on what you're trying to do with an image.
		 colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;             //don't care what previous layout the image was in
		 colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;			// the image to be ready for presentation using the swap chain after rendering, which is why we use

		 //Atacchment reference uses an atachment index that refers to index atacchment list passed to rendePaseCreateInfo. 
		 //The attachment parameter specifies which attachment to reference by its index in the attachment
		 //descriptions array. Our array consists of a single VkAttachmentDescription, so its index is 0. 
		 //The layout specifies which layout we would like the attachment to have during a subpass that uses this reference.
		 //Vulkan will automatically transition the attachment to this layout when the subpass is started. 
		 //We intend to use the attachment to function as a color buffer and the VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL layout will give us the best performance,as its name implies.
		 VkAttachmentReference colorAttachmentRef{};
		 colorAttachmentRef.attachment = 0;
		 colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		 VkSubpassDescription subpass{};
		 subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		 subpass.colorAttachmentCount = 1;
		 subpass.pColorAttachments = &colorAttachmentRef;

		 //Need to determinate when layout transitions ocurrs using subpass dependencies. 

		 //Conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGEL_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		 std::array<VkSubpassDependency, 2> subpassDependencies;
		 //Transition thaht must happen after
		 subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		 subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		 subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;	//Memory access. 
		 subpassDependencies[0].dependencyFlags = 0;	//Memory access. 

		 //Transition that must happen before....
		 subpassDependencies[0].dstSubpass = 0;
		 subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		 subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		 //Conversion from VK_IMAGEL_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		 subpassDependencies[1].srcSubpass = 0;
		 subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		 subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;;	//Memory access. 
		 subpassDependencies[1].dependencyFlags = 0;

		 subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		 subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		 subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		 
		 VkRenderPassCreateInfo renderPassCreateInfo{};
		 renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		 renderPassCreateInfo.attachmentCount = 1;
		 renderPassCreateInfo.pAttachments = &colorAttachment;
		 renderPassCreateInfo.subpassCount = 1;
		 renderPassCreateInfo.pSubpasses = &subpass;
		 renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
		 renderPassCreateInfo.pDependencies = subpassDependencies.data();
		 
		 VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
		 
			 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createRenderPass](ERROR)-->Failed to create Render Pass\n" :
			 std::cout << "[VulkanSystemRendered_t::createRenderPass](INFO)-->Render Pass was be create with Success\n";
			 
	 }

	 VkShaderModule
	 VulkanSystemRendered_t::createShaderModule(const std::vector<char>& code)
	 {
		 VkShaderModuleCreateInfo shaderModuleInfo{};
		 shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		 shaderModuleInfo.codeSize = code.size();
		 shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		 VkShaderModule shaderModule;

		 VkResult result = vkCreateShaderModule(mainDevice.logicalDevice, &shaderModuleInfo, nullptr, &shaderModule);

		 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createShaderModule](ERROR)-->Creating ShaderModule FAIL" << std::endl :
								std::cout << "[VulkanSystemRendered_t::createShaderModule](INFO)-->Creating ShaderModule SUCCESS" << std::endl;


		 if (result == VK_SUCCESS)return shaderModule;
		 else return{};
	 }

	 void VulkanSystemRendered_t::recordCommands()
	 {
		 static int index_frame = 0;

		 VkCommandBufferBeginInfo bufferBeginInfo{};
		 bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		 bufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; //Buffer can be resubmitted when it has already sumbitted an is awaiting execution  

		 VkRenderPassBeginInfo renderPassBeginInfo{};
		 renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		 renderPassBeginInfo.renderPass = renderPass;
		 renderPassBeginInfo.renderArea.offset = { 0,0 };
		 renderPassBeginInfo.renderArea.extent = swapChainExtent;
		 VkClearValue clearValue[] = {
			 {0.6f,0.6f,0.4,1.0f}
		 };
		 renderPassBeginInfo.pClearValues = clearValue;
		 renderPassBeginInfo.clearValueCount = 1;

		 auto StartRecording = [&](const VkCommandBuffer& cmbuffer) {

			 if (index_frame >= swapChainFramebuffers.size())index_frame = 0;
			 renderPassBeginInfo.framebuffer = swapChainFramebuffers[index_frame];
			 //Start recording commands to commands buffer 
			 VkResult result = vkBeginCommandBuffer(cmbuffer, &bufferBeginInfo);

			 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::recordCommands](ERROR)-->BeginCommandBuffer error to create it.\n" :
						std::cout << "[VulkanSystemRendered_t::recordCommands](INFO)-->Begin Command Buffer with success\n";
			 
			 //Start Begin Render Pass 
			 vkCmdBeginRenderPass(cmbuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);			// The render pass commands will be embedded in the primary command buffer itself and no secondary command buffers will be executed.

			 //Bind pipeline to be used in render pass 
			 vkCmdBindPipeline(cmbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			 vkCmdDraw(cmbuffer, 3, 1, 0, 0);

			 //End RenderPass 
			 vkCmdEndRenderPass(cmbuffer);

			 //End Recording 
			 result = vkEndCommandBuffer(cmbuffer);
			 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::recordCommands](ERROR)-->EndCommandBuffer error to create it.\n" :
				 std::cout << "[VulkanSystemRendered_t::recordCommands](INFO)--End Command Buffer with success\n";
			
			 index_frame++;
		 };
		 std::for_each(commandBuffers.begin(), commandBuffers.end(), StartRecording);
	 }

	 void 
	 VulkanSystemRendered_t::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
	 {
		 auto fun = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		 if (fun != nullptr) {
			 return fun(instance, messenger, pAllocator);
		 }
		 else
		 {
			 std::cout << "[VulkanSystemRendered_t::DestroyDebugUtilsMessengerEXT](Error)-->Fail to destroy debug messenger" << std::endl;
		 }
	 }

	 void 
	 VulkanSystemRendered_t::cleanFrameBuffers(std::vector<VkFramebuffer>* frameBuffers)
	 {
		 for (const auto& framebuffer : *frameBuffers) {

			 vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
		 }
	 }

	 QueueFamilyIndices
	 VulkanSystemRendered_t::getQueueFamilies(VkPhysicalDevice device)
	 {
		 QueueFamilyIndices indicesQueue;

		 uint32_t countQueueFamilies = 0;
		 int      auxIndex = 0;
		 vkGetPhysicalDeviceQueueFamilyProperties(device, &countQueueFamilies, nullptr);

		 std::vector<VkQueueFamilyProperties> queuefamilypropertiesList(countQueueFamilies);
		 vkGetPhysicalDeviceQueueFamilyProperties(device, &countQueueFamilies, queuefamilypropertiesList.data());

		 //Go through each queue family and check if it hast at least 1 of  the requieres type of queue.
		 for (const auto& queueFamily : queuefamilypropertiesList) {

			 if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

				 indicesQueue.graphicsFamily = auxIndex;

			 }


			 VkBool32 presentationSupport = false;
			 vkGetPhysicalDeviceSurfaceSupportKHR(device,auxIndex, surface,&presentationSupport);
			 
			 //Check if queue is presentation type (queue can be both graphics and presentation)
			 if (queueFamily.queueCount > 0 && presentationSupport == true) {

				 indicesQueue.presentationFamily = auxIndex;
			 }


			 if (indicesQueue.isValid()) {
				 
				 std::cout<<"[VulkanSystemRendered_t::getQueueFamilies](INFO)-->queue family indices are valid. stop searching\n";
				 break;
			 }
			 auxIndex++;
		 }
		 
		 return indicesQueue;

	 }

	
	void 
	VulkanSystemRendered_t::init(GLFWwindow* newWindow) noexcept {

		if (newWindow != nullptr) {
			window = newWindow;

			if (createInstance() == EXIT_SUCCESS) {
				setupDebugMessenger();
				createSurface();
				getPhysicalDevice();
				createLogicalDevice();
				createSwapChain();
				createRenderPass();
				createGraphicsPipeLine();
				createFramebuffers();
				createCommandPool();
				createCommandBuffer();
				recordCommands();

				std::cout << "[VulkanSystemRendered_t::init](INFO)-->Vulkan is SUCCES" << std::endl;
			}
			else {
				std::cout << "[VulkanSystemRendered_t::init](ERROR)-->Cant create instance Vulkan now " << std::endl;
			}
		}
	}

	
	const void 
	VulkanSystemRendered_t::cleanAll()
	{
		vkDestroyCommandPool(mainDevice.logicalDevice, commandPool, nullptr);
		cleanFrameBuffers(&swapChainFramebuffers);
		vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
		vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);
		for (auto image : swapChainImages) {

			vkDestroyImageView(mainDevice.logicalDevice,image.imageView, nullptr);
		}
		vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(mainDevice.logicalDevice, nullptr);
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroyInstance(instance, nullptr);
	}

	
	void VulkanSystemRendered_t::createCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};

		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  //Recorder a command buffer  every frame. 
		commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(indicesQueueFamily.graphicsFamily);

		VkResult result = vkCreateCommandPool(mainDevice.logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool);

		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createCommandPool](ERROR)-->Creating commandPool FAIL" << std::endl :
			std::cout << "[VulkanSystemRendered_t::createCommandPool](INFO)-->Creating Command Pool SUCCESS" << std::endl;
	}

	bool
	VulkanSystemRendered_t::createInstance() 
	{
		//1.Check if validationLayers are availabe and DebugMode is active. 
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			std::cout << "[VulkanSystemRendered_t::createInstance](WARNING!)-->Validation Layer Requested, but not available" << std::endl;
		}

		//2.Debuggin information logs no affects program.
		VkApplicationInfo appinfo = {};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pApplicationName = "Vulkan aplication";
		appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.pEngineName = "No engine";						   //Custom name. 
		appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		   //Custom field
		appinfo.apiVersion = VK_API_VERSION_1_0;			       //Vulkan Version

		//3.Create Information for Vulkan Instance VkInstance. 

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo =  &appinfo;
		//3.1 Get the extensions available to use. 
		extensionsAvailable = setUpExtensionNames_VkCreateInfo();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsAvailable.size());
		createInfo.ppEnabledExtensionNames = extensionsAvailable.data();
		//3.2 check if we have to create Layer info and use debug information to see instance debug created. 
		if (enableValidationLayers == true) {
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
			createInfo.pNext = nullptr;
		}
		//4. Create instance now. 
		std::cout << "[VulkanSystemRendered_t::createInstance](INFO)-->Creating Instance......" << std::endl;
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		
		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createInstance](ERROR)-->Creating Instance FAIL" << std::endl :
							   std::cout << "[VulkanSystemRendered_t::createInstance](INFO)-->Creating Instance SUCCESS" << std::endl;
		
		return result;

	}

	void 
	VulkanSystemRendered_t::createFramebuffers()
	{
		//Resize chainswapframebuffers
		swapChainFramebuffers.resize(swapChainImages.size());

		//Create framebuffer from them images. 
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			
			//Create Attachment 
			std::array<VkImageView, 1> attachment = { swapChainImages[i].imageView };

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;										//RenderPass Layout the frame buffer will be used with. 
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachment.size());
			framebufferCreateInfo.pAttachments = &attachment[0];								//List of attatchment ( 1:1 with Render pass 
			framebufferCreateInfo.width = swapChainExtent.width;								//frame buffer witdh 
			framebufferCreateInfo.height = swapChainExtent.height;								// frame buffer height; 
			framebufferCreateInfo.layers = 1;													

			
			VkResult result = vkCreateFramebuffer(mainDevice.logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]);


			result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createFramebuffers](ERROR)-->Creating Framebuffer number =" << i << "FAIL" << std::endl :
				std::cout << "[VulkanSystemRendered_t::createFramebuffers](INFO)-->Creating Framebuffer number = " << i << "  SUCCESS" << std::endl;

		}
	}


	void
	VulkanSystemRendered_t::createLogicalDevice()
	{
		//1 Complete fields with QueueFamilyIindices. and priority. 
		const float priority = 1.0f; //Vulkan needs to know how to handle multiples queue, (1.0 High priority)

		//1.1 Vector for queueCreateInfo  and set for family devices. 
		std::vector<VkDeviceQueueCreateInfo> queueCreatesInfo{};
		std::set<int> queueFamilyIndices = { indicesQueueFamily.graphicsFamily,indicesQueueFamily.presentationFamily };

		//1.2 Queue the logical devices needs to create and info to do. 

		for (int queueindex : queueFamilyIndices) {
			VkDeviceQueueCreateInfo queuedeviceInfo{};
			queuedeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queuedeviceInfo.pNext = nullptr;
			queuedeviceInfo.flags = 0;
			queuedeviceInfo.queueFamilyIndex = queueindex;
			queuedeviceInfo.queueCount = 1;
			queuedeviceInfo.pQueuePriorities = &priority;

			queueCreatesInfo.push_back(queuedeviceInfo);
		}

		//2.Create Device Info. 
		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreatesInfo.size());						//Only have one VkDeviceQueueCreateInfo. 
		deviceInfo.pQueueCreateInfos = &queueCreatesInfo[0];			//List of queue create 
		deviceInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());                       //Number of enabled logical device extensions 
		deviceInfo.ppEnabledExtensionNames = &deviceExtensions[0];               //Lit of enabled logical devices extensions. 

		//2.1 Create Physical Devices Features.This point is important to get functionallyty to next use in Rasterization Pipelines. 
		VkPhysicalDeviceFeatures devicesPhyFeatures{};

		deviceInfo.pEnabledFeatures = &devicesPhyFeatures;

		VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);

		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createLogicalDevice](ERROR)-->Failed to create Logical Device\n" :
			std::cout << "[VulkanSystemRendered_t::createLogicalDevice](INFO)-->Logical device was be create with Success\n";

		//3. Create handle queue. to reference a VkQueue, not problem is graphicsQueue its handle the same than presentationQueue. ItsOK. 
		vkGetDeviceQueue(mainDevice.logicalDevice, indicesQueueFamily.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(mainDevice.logicalDevice, indicesQueueFamily.presentationFamily, 0, &presentationQueue);


	}

	void
	VulkanSystemRendered_t::createSurface()
	{
		//Create Surface, This return info struct , return results VkResutls.  
		VkResult result =  glfwCreateWindowSurface(instance, window, nullptr, &surface);


		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createSurface](ERROR)-->Creating Source FAIL" << std::endl :
			std::cout << "[VulkanSystemRendered_t::createSurface](INFO)-->Creating Source SUCCESS" << std::endl;
	}

	
	void 
	VulkanSystemRendered_t::getPhysicalDevice()
	{
		// Enumerate Physical devices the vkInstance can access
		uint32_t deviceCount = 0;
		int      auxcount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// If no devices available, then none support Vulkan!
		if (deviceCount == 0)
		{
			std::cout << "[VulkanSystemRendered_t::getPhysicalDevice]:(ERROR)-->No devices support Vulkan\n";
		}

		// Get list of Physical Devices
		std::vector<VkPhysicalDevice> deviceList(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());


		for (const auto& device : deviceList) {

			if (checkDeviceSuitable(device)) {

				//We choose the first device that have the properties that vulkan needit, 
				//If its 2 gpus or more, TODO: map with score to choose the better options. 
				mainDevice.physicalDevice = device;
				std::cout << "[VulkanSystemRendered_t::getPhysicalDevice]:(Info)-->Devices support Vulkan\n";
				
				break;
			}
			
		}
		
	}//end namespace

	void
	VulkanSystemRendered_t::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}
	


}