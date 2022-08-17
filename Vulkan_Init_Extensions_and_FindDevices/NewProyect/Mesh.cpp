#include "Mesh.h"

namespace VK_Test {
    

    Mesh::Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, std::vector<Vertex>* vertices) : physicalDevice(newPhysicalDevice),
        Device(newDevice)
    {
        VertexCount = static_cast<int>(vertices->size());
        createVertexBuffer(vertices);
    }

    int Mesh::gentVertexCount() const
    {
        return VertexCount;
    }

    VkBuffer Mesh::getVertexBuffer() const
    {
        return vertexBuffer;
    }

    void Mesh::DestroyVertexBuffer()
    {
        vkDestroyBuffer(Device, vertexBuffer, nullptr);
        vkFreeMemory(Device, vertexBufferMemory, nullptr);

    }

    Mesh::~Mesh()
    {
    }
    uint32_t Mesh::findMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const 
    {
        //Get Properties. 
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount ; i++) {

            if (allowedTypes & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {

                return i;
            }
       }


        return {};
    }
    VkBuffer Mesh::createVertexBuffer(std::vector<Vertex>* vertices)
    {
        //Informatio to crete buffer Info ( Not include assigning memory) 
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = sizeof(Vertex) * vertices->size();              //Size 1 vertex * (number of total vertices) 
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;             // Multiple buffer usages, but we want use with Vertex. 
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;               // Exclusive only, Similar like SwapChainImages.

        VkResult result = vkCreateBuffer(Device, &bufferCreateInfo, nullptr, &vertexBuffer);

        result != VK_SUCCESS ? std::cout << "[Mesh::createVertexBuffer](ERROR)-->CreateBuffer error to create it.\n" :
            std::cout << "[Mesh::createVertexBuffer](INFO)-->CreateBuffer Buffer with success\n";
        
        //Get buffer Memory Requierements
        VkMemoryRequirements memRequierements{};
        vkGetBufferMemoryRequirements(Device, vertexBuffer, &memRequierements);

        //ALLOCATE memory to buffer. 
        VkMemoryAllocateInfo memAllocateInfo{};
        memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAllocateInfo.allocationSize = memRequierements.size;
        memAllocateInfo.memoryTypeIndex = findMemoryTypeIndex(memRequierements.memoryTypeBits, 
            (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));              //HOST_VISIBLE_BIT : CPU can interect with memory buffer. 
                                                                                                        //HOST_COHERENT_BIT : Allows placemente of data straight into  buffer after mapping.  
        //Allocate memory to VkDeviceMemory

        result = vkAllocateMemory(Device, &memAllocateInfo, nullptr, &vertexBufferMemory);

        result != VK_SUCCESS ? std::cout << "[Mesh::createVertexBuffer](ERROR)-->Allocate Memory error process.\n" :
            std::cout << "[Mesh::createVertexBuffer](INFO)-->Allocate Memory Sucess\n";

        //Allocate memory given to vertex buffer. 

        result = vkBindBufferMemory(Device, vertexBuffer, vertexBufferMemory, 0);

        //MAP MEMORY TO VERTEX BUFFER. 
        void* data;
        vkMapMemory(Device, vertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);                    //2.Map the vertex buffer memory to that point 
        memcpy(data, vertices->data(), (size_t)bufferCreateInfo.size);



        vkUnmapMemory(Device, vertexBufferMemory);

       
        return{};
    }
}