// AoRenderPasses.h
// This file will hold an enum to keep track of the different renderpasses and attachments an AO renderer needs

#ifndef _AO_RENDER_PASSES_
#define _AO_RENDER_PASSES_

namespace DDM
{
	enum
	{
		kSubpass_DEPTH = 0,
		kSubpass_GBUFFER = 1,
		kSubpass_AO_GEN = 2,
		kSubpass_AO_BLUR = 3,
		kSubpass_LIGHTING = 4,
		kSubpass_IMGUI = 5
	};

	enum
	{
		kAttachment_BACK = 0,
		kAttachment_DEPTH = 1,
		kAttachment_GBUFFER_ALBEDO = 2,
		kAttachment_GBUFFER_VIEWNORMAL = 3,
		kAttachment_GBUFFER_VIEWPOS = 4,
		kAttachment_AO_MAP = 5,
		kAttachment_AO_BLURRED = 6
	};
}

#endif // !_AO_RENDER_PASSES_
