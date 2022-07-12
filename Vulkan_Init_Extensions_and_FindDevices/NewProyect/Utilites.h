#pragma once
#include <vector>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


namespace VK_Test {

	
	
	using VectExtensions = std::vector<const char*>;

	typedef struct MainDevice {
		VkPhysicalDevice	physicalDevice;
		VkDevice			logicalDevice;
	};
	struct QueueFamilyIndices {

		//-Constructor
		explicit QueueFamilyIndices() = default;

		int graphicsFamily = -1;

		/// <summary>
		/// Chek if queue family is valid
		/// </summary>
		/// <returns></returns>
		bool isValid() {

			return graphicsFamily  >= 0 ? true : false;

		}

	};//end queuefamiliesIndices. 



}//end namespace 