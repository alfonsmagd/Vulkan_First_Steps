#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


namespace VK_Test {

	
	//--Using definitions--//
	using VectExtensions = std::vector<const char*>;
	using ArrayVertex = std::array<VkVertexInputAttributeDescription, 1>;
	
	//--Const--//
	const int MAX_FRAME_DRAWS = 2;
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

	static std::vector<char> readFromFile(const std::string& filename) {

		// Open stream from given filename
		//std::ios::binary , tells stream to read filename  as binary.
		//std::ios::ate	   , tells stream to start reading from end file 
		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		if (!file.is_open()) {
			std::cout << "[readFromFile]:(ERROR)cant open the file.\n";
		}
		//store the size to create a vector with same size that file. 
		//Get current read position and use 
		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> fileBuffer(fileSize);

		//move the start of the file.
		file.seekg(0);
		file.read(&fileBuffer[0], fileSize);
		file.close();

		return fileBuffer;


	}

	struct Vertex {

		glm::vec3 pos;
		glm::vec3 col;
	};

}//end namespace 