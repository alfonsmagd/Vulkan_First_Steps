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

		 //To choose how many images are in the swap chain . One more that the minimum to allow triple buffering. 
		 uint32_t imageCount = swapchainDetails.surfaceCapabilites.minImageCount + 1;

		 //If imageCounter is higher than max, then clamp down to max. 
		 if (imageCount > swapchainDetails.surfaceCapabilites.maxImageCount && 
			 swapchainDetails.surfaceCapabilites.minImageCount ) {

			 imageCount = swapchainDetails.surfaceCapabilites.maxImageCount;
		 }
		    
			 //Create Information for Swap chain 
			 VkSwapchainCreateInfoKHR swapChainInfo{};

			 createSwapChainInfo(bestSurfaceFormat, bestPresentMode, extent2D, swapchainDetails, imageCount, &swapChainInfo);

			 VkResult result =   vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapChainInfo, nullptr, &swapchain);

			 result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createSwapChain](ERROR)-->Failed to create SwapChain\n" :
				 std::cout << "[VulkanSystemRendered_t::createSwapChain](INFO)-->SwapChain  was be create with Success\n";

			 //Get Volkan componentes to use after. 
			 if (result == VK_SUCCESS) {

				 swapChainImageFormat = bestSurfaceFormat.format;
				 swapChainExtent = extent2D;

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
		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(mainDevice.logicalDevice, nullptr);
		vkDestroyInstance(instance, nullptr);
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

		//2.1 Create Physical Devices Features. 
		VkPhysicalDeviceFeatures devicesPhyFeatures{};
		deviceInfo.pEnabledFeatures = &devicesPhyFeatures;

		VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);

		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createLogicalDevice](ERROR)-->Failed to create Logical Device\n" :
			std::cout << "[VulkanSystemRendered_t::createLogicalDevice](INFO)-->Logical device was be create with Success\n";

		//3. Create handle queue. to reference a VkQueue, not problem is graphicsQueue its handle the same than presentationQueue. ItsOK. 
		vkGetDeviceQueue(mainDevice.logicalDevice, indicesQueueFamily.graphicsFamily, 0, &graphicsQueue);
		vkGetDeviceQueue(mainDevice.logicalDevice, indicesQueueFamily.presentationFamily, 0, &presentationQueue);


	}

	void VulkanSystemRendered_t::createSurface()
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