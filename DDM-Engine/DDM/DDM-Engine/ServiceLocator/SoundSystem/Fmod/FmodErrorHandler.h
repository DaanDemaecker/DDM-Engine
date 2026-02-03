// FmodErrorHandler.h
// This class serves for the single purpose of displaying FMOD errors in the console

#ifndef _DDM_FMOD_ERROR_HANDLER_
#define _DDM_FMOD_ERROR_HANDLER_


// File includes
#include "DDM-Engine/Includes/FmodIncludes.h"

// Standard library includes
#include <iostream>

namespace DDM
{
	static void HandleError(FMOD_RESULT result)
	{
		if (result == FMOD_OK)
		{
			return;
		}

		std::cout << "Fmod error: " << result << "\n";
	}
}

#endif // !_DDM_FMOD_ERROR_HANDLER_