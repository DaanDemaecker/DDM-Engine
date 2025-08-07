// AoRenderPasses.h
// This file will hold an enum to keep track of the different renderpasses an AO renderer needs

#ifndef _AO_RENDER_PASSES_
#define _AO_RENDER_PASSES_

namespace DDM3
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
}

#endif // !_AO_RENDER_PASSES_
