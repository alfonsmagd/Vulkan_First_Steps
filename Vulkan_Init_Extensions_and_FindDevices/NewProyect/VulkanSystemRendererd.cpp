#include "VulkanSystemRendererd.h"


namespace VK_Test {
	
	bool VulkanSystemRendered_t::chekInstanceExtensionsSupports(VectExtensions* checkExtensions)
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
	 VulkanSystemRendered_t::setUpExtensionNames_VkCreateInfo(VkInstanceCreateInfo* cinfo) {

		auto instanceExtensions = VectExtensions();
		uint32_t glfwExtensionsCount = 0;

		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

		for (size_t i = 0; i < glfwExtensionsCount; ++i) {
			instanceExtensions.push_back(glfwExtensions[i]);
		}
		//Chek Instance Extensions is supported. 
		if (!chekInstanceExtensionsSupports(&instanceExtensions)) {

			std::cout <<"Vulkan not have extenions correctly \n";

			return{};
		}

		else {
			std::cout<< "Vulkan have correctly extension to work\n";
			
			cinfo->enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
			cinfo->ppEnabledExtensionNames = instanceExtensions.data();

			return instanceExtensions;
		}
		
		
	}

	
	void VulkanSystemRendered_t::init(GLFWwindow* newWindow) noexcept {

		if (newWindow != nullptr) {
			window = newWindow;

			if (createInstance() == EXIT_SUCCESS) {
				getPhysicalDevice();
				std::cout << "CreateInstance Vulkan is SUCCES" << std::endl;
			}
			else {
				std::cout << "Cant create instance Vulkan now " << std::endl;
			}
		}
	}

	
	const void VulkanSystemRendered_t::cleanAll()
	{
		vkDestroyInstance(instance, nullptr);
	}

	bool VulkanSystemRendered_t::createInstance() 
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
		auto extensionsAviliable = setUpExtensionNames_VkCreateInfo(&createInfo);
		//TODO: Not to implement yet. 
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		//Create instance now. 
		if (vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS)
			return VK_SUCCESS;
		
		return true;

	}

	void VulkanSystemRendered_t::getPhysicalDevice()
	{
		// Enumerate Physical devices the vkInstance can access
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		// If no devices available, then none support Vulkan!
		if (deviceCount == 0)
		{
			std::cout << "No devices support Vulkan\n";
		}

		// Get list of Physical Devices
		std::vector<VkPhysicalDevice> deviceList(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());
	}

	


}