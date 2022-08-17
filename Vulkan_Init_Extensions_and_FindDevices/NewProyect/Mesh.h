#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "Utilites.h"


namespace VK_Test {

	class Mesh
	{
	public:
		//-Constructor-//
		explicit Mesh() = default;;
		Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices);

		//--Public Functions--//
		int gentVertexCount() const ;
		VkBuffer getVertexBuffer() const;
		void DestroyVertexBuffer();

		//Mesh(const Mesh&) = delete;
		//Mesh& operator=(const Mesh&) = delete;
		~Mesh();


	private:
		int VertexCount;
		VkPhysicalDevice physicalDevice;
		VkDevice		 Device; 
		VkBuffer		 vertexBuffer;
		VkDeviceMemory   vertexBufferMemory;


		//-Private-Functions // 

		uint32_t findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const ;

		/// <summary>
		/// Create Vertex Buffer
		/// </summary>
		/// <param name="vertices"></param>
		VkBuffer createVertexBuffer(std::vector<Vertex>* vertices);

		


	};

}
