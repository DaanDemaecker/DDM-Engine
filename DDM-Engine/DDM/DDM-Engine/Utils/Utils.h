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

	/// <summary>
	/// Get a random float
	/// </summary>
	/// <param name="min: ">minimum float value</param>
	/// <param name="max: ">maximum float value</param>
	/// <returns>Random float</returns>
	float RandomFLoat(float min, float max);

	/// <summary>
	/// Lerps a value between a min and max given a factor
	/// </summary>
	/// <param name="min: ">Lower value</param>
	/// <param name="max: ">Upper value</param>
	/// <param name="factor: ">Factor between 0 and 1</param>
	/// <returns>Lerped value</returns>
	float Lerp(float min, float max, float factor);
}

#endif // !UtilsIncluded