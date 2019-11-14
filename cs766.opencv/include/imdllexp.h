#pragma once

// Define the macro for DLL exports
#ifndef SWIG
#	ifdef WIN32
#		ifdef IMAGEMAN_EXPORTS
#			define IM_DLL __declspec(dllexport)
#			define IM_STL_DLL
#		else
#			define IM_DLL __declspec(dllimport)
#			define IM_STL_DLL extern
#			ifdef  _DEBUG
#				pragma comment(lib, "ImageMan_d.lib")
#			else
#				pragma comment(lib, "ImageMan_r.lib")
#			endif
#		endif
#	else
#		ifdef _MSC_VER	// -hupp
#			error	"You don't have WIN32 defined, but you're using Visual Studio.  Things will probably break."
#		else
#			define IM_DLL
#			define IM_STL_DLL
#		endif
#	endif
#else // SWIG
#	define IM_DLL
#	define IM_STL_DLL
#endif
