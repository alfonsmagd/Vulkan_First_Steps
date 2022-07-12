#include "VulkanSystemRendererd.h"


namespace VK_Test {
	
	
	HANDLE STD_OutputColor = GetStdHandle(STD_OUTPUT_HANDLE);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
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

		 return indicesQueueFamily.isValid();
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

	 void VulkanSystemRendered_t::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
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
				getPhysicalDevice();
				createLogicalDevice();
				
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

		//1.1 Queue the logical devices needs to create and info to do. 
		VkDeviceQueueCreateInfo queuedeviceInfo{};
		queuedeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queuedeviceInfo.pNext = nullptr;
		queuedeviceInfo.flags = 0;
		queuedeviceInfo.queueFamilyIndex = indicesQueueFamily.graphicsFamily;
		queuedeviceInfo.queueCount = 1;
		queuedeviceInfo.pQueuePriorities = &priority;
		

		//2.Create Device Info. 
		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = 1;						//Only have one VkDeviceQueueCreateInfo. 
		deviceInfo.pQueueCreateInfos = &queuedeviceInfo;			//List of queue create 
		deviceInfo.enabledExtensionCount = 0;                       //Number of enabled logical device extensions 
		deviceInfo.ppEnabledExtensionNames = nullptr;               //Lit of enabled logical devices extensions. 

		//2.1 Create Physical Devices Features. 
		VkPhysicalDeviceFeatures devicesPhyFeatures{};
		deviceInfo.pEnabledFeatures = &devicesPhyFeatures;

		VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceInfo, nullptr, &mainDevice.logicalDevice);

		result != VK_SUCCESS ? std::cout << "[VulkanSystemRendered_t::createLogicalDevice](ERROR)-->Failed to create Logical Device\n" :
			std::cout << "[VulkanSystemRendered_t::createLogicalDevice](INFO)-->Logical device was be create with Success\n";

		//3. Create handle queue. to reference a VkQueue
		vkGetDeviceQueue(mainDevice.logicalDevice, indicesQueueFamily.graphicsFamily, 0, &graphicsQueue);

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