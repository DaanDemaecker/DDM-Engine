#ifndef _DDM_EXPORT_
#define _DDM_EXPORT_

#if defined(_WIN32)
	#if defined(DDMENGINE_BUILD)
		#define DDM_API __declspec(dllexport)
	#else
		#define DDM_API __declspec(dllimport)
	#endif
#else
	#define DDM_API
#endif

#endif // !_DDMML_EXPORT_
