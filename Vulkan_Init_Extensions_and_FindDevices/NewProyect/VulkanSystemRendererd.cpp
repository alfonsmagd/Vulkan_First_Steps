#include "VulkanSystemRendererd.h"


namespace VK_Test {
	
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
	 VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo(VkInstanceCreateInfo& cinfo) {

		auto instanceExtensions = VectExtensions();
		uint32_t glfwExtensionsCount = 0;

		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		for (size_t i = 0; i < glfwExtensionsCount; ++i) {
			instanceExtensions.push_back(glfwExtensions[i]);
		}
		//Chek Instance Extensions is supported. 
		if (!chekInstanceExtensionsSupports(&instanceExtensions)) {
			std::cout <<"[VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo](ERROR)-->Vulkan not have extenions correctly\n";
			return{};
		}

		else {
			std::cout<< "[VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo](INFO)-->Vulkan have correctly extension to work\n";
			cinfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
			cinfo.ppEnabledExtensionNames = instanceExtensions.data();

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
		vkDestroyDevice(mainDevice.logicalDevice, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	bool 
	VulkanSystemRendered_t::createInstance() 
	{
		//Debuggin information logs no affects program.
		VkApplicationInfo appinfo = {};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pApplicationName = "Vulkan aplication";
		appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.pEngineName = "No engine";						   //Custom name. 
		appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		   //Custom field
		appinfo.apiVersion = VK_API_VERSION_1_0;			       //Vulkan Version

		//Create Information for Vulkan Instance VkInstance. 

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo =  &appinfo;
		extensionsAviliable = setUpExtensionNames_VkCreateInfo(createInfo);
		//TODO: Not to implement yet. 
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		//Create instance now. 
		if (vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS)
			return VK_SUCCESS;
		
		return true;

	}

	void
	VulkanSystemRendered_t::createLogicalDevice()
	{
		//1.1 Complete fields with QueueFamilyIindices. and priority. 
		const float priority = 1.0f; //Vulkan needs to know how to handle multiples queue, (1.0 High priority)

		//1.Queue the logical devices needs to create and info to do. 
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

	


}