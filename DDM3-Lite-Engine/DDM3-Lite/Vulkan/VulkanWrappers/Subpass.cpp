// Subpass.cpp

#include "Subpass.h"

#include <iostream>

DDM3::Subpass::~Subpass()
{
}

VkSubpassDescription DDM3::Subpass::GetDescription()
{
	SetupDescription();

	return m_Description;
}

void DDM3::Subpass::AddInputReference(VkAttachmentReference reference)
{
	m_InputAttachmentRefs.push_back(reference);
}

void DDM3::Subpass::AddReference(VkAttachmentReference reference)
{
	m_AttachmentReferences.push_back(reference);
}

void DDM3::Subpass::AddDepthRef(VkAttachmentReference reference)
{
	m_DepthAttachmentRef = reference;
	m_DepthRefSet = true;
}

void DDM3::Subpass::AddResolveRef(VkAttachmentReference reference)
{
	m_ResolveAttachmentRef = reference;
	m_ResolveSet = true;
}

void DDM3::Subpass::SetupDescription()
{

	m_Description.flags = 0;
	m_Description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	m_Description.inputAttachmentCount = m_InputAttachmentRefs.size();
	m_Description.pInputAttachments = m_InputAttachmentRefs.size() > 0 ? m_InputAttachmentRefs.data() : nullptr;
	m_Description.colorAttachmentCount = m_AttachmentReferences.size();
	m_Description.pColorAttachments = m_AttachmentReferences.size() > 0 ? m_AttachmentReferences.data() : nullptr;
	m_Description.pResolveAttachments = m_ResolveSet ? &m_ResolveAttachmentRef : nullptr;
	m_Description.pDepthStencilAttachment = m_DepthRefSet ? &m_DepthAttachmentRef : nullptr;
	m_Description.preserveAttachmentCount = 0;
	m_Description.pPreserveAttachments = nullptr;
}
