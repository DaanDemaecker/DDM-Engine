// Buffer.cpp

// Header include
#include "Buffer.h"

// File includes
#include "DataTypes/Structs.h"

template <>
DDM::Buffer<DDM::Vertex>::Buffer(std::vector<DDM::Vertex>& data)
{
    // Copy over data
    m_Data.resize(data.size());
    std::copy(data.begin(), data.end(), m_Data.data());

    // Create vertex buffer
    VulkanObject::GetInstance().CreateVertexBuffer(m_Data, m_Buffer, m_BufferMemory);
}

template <>
DDM::Buffer<uint32_t>::Buffer(std::vector<uint32_t>& data)
{
    // Copy over data
    m_Data.resize(data.size());
    std::copy(data.begin(), data.end(), m_Data.data());

    // Create index buffer
    VulkanObject::GetInstance().CreateIndexBuffer(m_Data, m_Buffer, m_BufferMemory);
}