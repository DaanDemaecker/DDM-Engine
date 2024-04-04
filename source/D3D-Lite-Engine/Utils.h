#ifndef UtilsIncluded
#define UtilsIncluded
#include "Structs.h"

#include <vector>
#include <fstream>

namespace Utils
{
	std::vector<char> readFile(const std::string& filename);

	void LoadModel(const std::string& filename, std::vector<D3D::Vertex>& vertices, std::vector<uint32_t>& indices);
}

#endif // !UtilsIncluded