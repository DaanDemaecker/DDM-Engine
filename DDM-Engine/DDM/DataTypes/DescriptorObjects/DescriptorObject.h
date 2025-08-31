// DescriptorObject.h
// This class will serve as the base class for descriptor object classes
// Descriptor object classes will be needed to automate descriptor set updating

#ifndef _DESCRIPTOR_OBJECT_
#define _DESCRIPTOR_OBJECT_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard libary includes
#include <vector>

namespace DDM
{
	class DescriptorObject
	{
	public:
		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="type:">type of descriptor this object will hold</param>
		DescriptorObject(VkDescriptorType type)
			:m_Type{ type }
		{

		}

		// Delete default constructor
		DescriptorObject() = delete;

		/// <summary>
		/// Default destructor
		/// </summary>
		virtual ~DescriptorObject() = default;

		// Rule of 5
		DescriptorObject(DescriptorObject& other) = delete;
		DescriptorObject(DescriptorObject&& other) = delete;

		DescriptorObject& operator=(DescriptorObject& other) = delete;
		DescriptorObject& operator=(DescriptorObject&& other) = delete;

		/// <summary>
		/// Add the descriptor write objets to the list of descriptorWrites
		/// </summary>
		/// <param name="descriptorSet: ">current descriptorset connected to this descriptor object</param>
		/// <param name="descriptorWrites: ">list of descriptorWrites to add to</param>
		/// <param name="binding: ">current binding in the shader files</param>
		/// <param name="amount: ">amount of elements in the array at this binding</param>
		/// <param name="index">index of the current frame in flight</param>
		virtual void AddDescriptorWrite(VkDescriptorSet descriptorSet, std::vector<VkWriteDescriptorSet>& descriptorWrites, int& binding, int amount, int index) = 0;

	protected:
		// The type of descriptor this object will hold
		VkDescriptorType m_Type{};

	};
}
#endif // !_DESCRIPTOR_OBJECT_