// Subpass.h
// This class will serve as a wrapper 

#ifndef _SUBPASS_
#define _SUBPASS_

// File includes
#include "Includes/VulkanIncludes.h"

// Standard library includes
#include <vector>

namespace DDM3
{
	class Subpass final
	{
	public:
		// Constructor
		Subpass() = default;

		// Destructor
		~Subpass();

		// Delete copy and move operations
		Subpass(Subpass&) = delete;
		Subpass(Subpass&&) = delete;
		Subpass& operator=(Subpass&) = delete;
		Subpass& operator=(Subpass&&) = delete;

		VkSubpassDescription GetDescription();

		void AddInputReference(VkAttachmentReference reference);
		void AddReference(VkAttachmentReference reference);
		void AddDepthRef(VkAttachmentReference reference);
		void AddResolveRef(VkAttachmentReference reference);
	private:
		VkSubpassDescription m_Description{};

		std::vector<VkAttachmentReference> m_InputAttachmentRefs{};

		std::vector<VkAttachmentReference> m_AttachmentReferences{};

		VkAttachmentReference m_DepthAttachmentRef{};
		bool m_DepthRefSet{ false };

		VkAttachmentReference m_ResolveAttachmentRef{};
		bool m_ResolveSet{ false };

		void SetupDescription();
	};
}

#endif // !_SUBPASS_
