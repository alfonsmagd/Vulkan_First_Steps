#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define NOMINMAX
#include <GLFW/glfw3.h>
#include <Windows.h>
#include "Utilites.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <optional>
#include <limits>
#include <set>
#include <tuple>



namespace VK_Test {


	class VulkanSystemRendered_t
	{

	public:
		//Constructor 
		explicit VulkanSystemRendered_t() = default;
		//Functions 

		/// <summary>
		/// Init the system VulkansystemRendered_t and createInstance
		/// </summary>
		/// <param name="newWindow">Its the window which will rendered and show all. </param>
		/// <returns>A message in console. about the succes or fail.</returns>
		void init(GLFWwindow* newWindow) noexcept;
		/// <summary>
		/// Destroy vulkan instance. 
		/// </summary>
		/// <returns></returns>
		const void cleanAll();

	private:
		//Variables Ptr
		GLFWwindow* window = nullptr;

		//Vulkan Components 
		VkPipeline graphicsPipeline;
		VkPipelineLayout pipelineLayout; 
		VkRenderPass renderPass;						
		VkInstance instance;							//Store the instance component;	
		VectExtensions extensionsAvailable;				//Store the exentsions aviliable
		QueueFamilyIndices indicesQueueFamily;			//Store the indicesQueueFamily its valid and count. 
		MainDevice mainDevice;							//Store the PhysicalDevice, and logical device. 
		VkQueue graphicsQueue;							//Store the graphics Queue. 
		VkQueue presentationQueue;
		VkDebugUtilsMessengerEXT debugMessenger;        //Store the Debug Utils messenger debug. 
		const VectExtensions validationLayers = { "VK_LAYER_KHRONOS_validation" };
		VkSurfaceKHR surface;							//Store surface when images will be render. 
		VkSwapchainKHR swapchain;
		
		//--Vector Components buffer process. 
		std::vector<VkFramebuffer> swapChainFramebuffers; //Vecto to store framebuffer images 
		std::vector<SwapChainImage> swapChainImages;    //Vector to store swapChain images.
		std::vector<VkCommandBuffer> commandBuffers;    //Vector to store command buffers 

		//-Utilites Components.
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		//-Pools
		VkCommandPool commandPool;

		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////////////////////////
		/******************STATIC FUNCTIONS**********************************************************/
		//////////////////////////////////////////////////////////////////////////////////////////////
		// 


		void setupDebugMessenger();
		//Vulkan Functions.
		// -Initialize and create functions

		void createCommandBuffer();
		/// <summary>
		/// 
		/// </summary>
		void createCommandPool();
		/// <summary>
			/// 
			/// </summary>
			/// <returns></returns>
		bool createInstance();
		/// <summary>
		/// 
		/// </summary>
		void createFramebuffers();
		/// <summary>
		/// Create logicalDevice to use the interface with physicalDevice. 
		/// </summary>
		void createLogicalDevice();
		/// <summary>
		/// 
		/// </summary>
		void createSurface();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="instance"></param>
		/// <param name="pCreateInfo"></param>
		/// <param name="pAllocator"></param>
		/// <param name="pDebugMessenger"></param>
		/// <returns></returns>
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger);
		/// <summary>
		/// Create Swapchain function.
		/// </summary>
		void createSwapChain();
		/// <summary>
		/// 
		/// </summary>
		/// <param name="surfaceFormat"></param>
		/// <param name="presentMode"></param>
		/// <param name="extent"></param>
		/// <param name="details"></param>
		/// <param name="imagecount"></param>
		/// <param name="swapchainInfo"></param>
		void createSwapChainInfo(const VkSurfaceFormatKHR& surfaceFormat, const VkPresentModeKHR& presentMode, VkExtent2D extent, const SwapChainDetails& details, const uint32_t imagecount, VkSwapchainCreateInfoKHR* swapchainInfo);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="image"></param>
		/// <param name="format"></param>
		/// <param name="flags"></param>
		/// <returns></returns>
		VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectflags);
		/// <summary>
		/// 
		/// </summary>
		void createGraphicsPipeLine();
		/// <summary>
		/// This render pass need to tell Vulkan about the framebuffer attachments that will be used while rendering. 
		 // We need to specify how many color and depth buffers there will be, 
		 // how many samples to use for each of them and how their contents should be handled throughout the rendering operations. 
		 //All of this information is wrapped in a render pass object,
		/// </summary>
		void createRenderPass();
		VkShaderModule createShaderModule(const std::vector<char>& code);

		//--Record Functions. 
		
		/// <summary>
		/// 
		/// </summary>
		void recordCommands();




		//Vulkan get information or devices.

			/// <summary>
			/// Get gpu device reference. 
			/// </summary>
		void getPhysicalDevice();
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
		//Vulkan functions supports. 

		VectExtensions setUpExtensionNames_VkCreateInfo();

		//--Check Vulkan functions. 

			/// <summary>
			/// Checks if all of the requested layers are available
			/// </summary>
			/// <returns>True --> Validation Layer is available . False--> Validation Layer is not available</returns>
		bool checkValidationLayerSupport();
		/// <summary>
		/// checking if its posible to switch on device to other gpu device.
		/// </summary>
		/// <param name="device">Device which will be check.</param>
		/// <returns>true -> The device is suitable , false-> the device isn´t suitable </returns>
		bool checkDeviceSuitable(VkPhysicalDevice device);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="device"></param>
		/// <returns></returns>
		bool checkDeviceExtensionsSupport(VkPhysicalDevice device);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="checkExtensions"></param>
		/// <returns></returns>
		bool const chekInstanceExtensionsSupports(VectExtensions* checkExtensions) const;

		//--Getter Vulkan functions. 

		auto getSwapChainDetails(VkPhysicalDevice device);
		/// <summary>
		/// Choose the best surfaceFormatKHR 
		/// Choose always format:		VK_FORMAT_R8G8B8A8_UNORM
		///				  colorSpace :  VK_COLOR_SPACE_SGRB_NONLINEAL
		/// </summary>
		/// <param name="formats">return a VkSurfaceFormatKHR the best in the vector. </param>
		/// <returns></returns>
		VkSurfaceFormatKHR getBetterSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>& formats);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="presentsModes"></param>
		/// <returns></returns>
		VkPresentModeKHR   getBetterPresentModeKHR(const std::vector<VkPresentModeKHR>& presentsModes);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="surfaceCapability"></param>
		/// <returns></returns>
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapability);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="instance"></param>
		/// <param name="messenger"></param>
		/// <param name="pAllocator"></param>
		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
			VkDebugUtilsMessengerEXT messenger,
			const VkAllocationCallbacks* pAllocator);
		void cleanFrameBuffers(std::vector<VkFramebuffer>* frameBuffers);
	};

}//end Namespace