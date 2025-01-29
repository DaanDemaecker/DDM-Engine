// Utils.h
// This file defines utility functions with the Utils namespace
// Currently these functions include reading in binary files and obj files

#ifndef UtilsIncluded
#define UtilsIncluded

// Standard library includes
#include <vector>
#include <fstream>

namespace Utils
{
	// Reads in a file and returns it as a char vector
	// This function is mainly used to read in shaders
	// Parameters:
	//     - filename: The name of the file to be read
	std::vector<char> readFile(const std::string& filename);

	// This function will read and return the extension of a given file name
	// Parameters:
	//     - filename: The name of the file
	std::string GetExtension(const std::string& filename);
}

#endif // !UtilsIncluded