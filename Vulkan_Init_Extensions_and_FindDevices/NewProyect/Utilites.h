#pragma once
#include <vector>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


namespace VK_Test {

	
	
	using VectExtensions = std::vector<const char*>;

	const VectExtensions deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	typedef struct MainDevice {
		VkPhysicalDevice	physicalDevice;
		VkDevice			logicalDevice;
	};
	struct QueueFamilyIndices {

		//-Constructor
		explicit QueueFamilyIndices() = default;

		int graphicsFamily = -1;
		int presentationFamily = -1;

		/// <summary>
		/// Chek if queue family is valid
		/// </summary>
		/// <returns></returns>
		bool isValid() {

			return graphicsFamily >= 0 && presentationFamily >= 0;

		}

	};//end queuefamiliesIndices. 
	struct SwapChainDetails {
		VkSurfaceCapabilitiesKHR surfaceCapabilites;	// Surface properties,e.g. image size/extends 
		std::vector<VkSurfaceFormatKHR> formats;		// Surface image formats , erg rgba and size of each color. 
		std::vector<VkPresentModeKHR> presentmode;      // How images should be presented to screen. 
	};

	struct SwapChainImage {
		VkImage image;
     	VkImageView imageView;
	};



}//end namespace 