// DDMFileSystem.cpp

// Header include
#include "DDMFileSystem.h"

// Standard library includes
#include <filesystem>

DDM::DDMFileSystem::DDMFileSystem()
{
}

DDM::DDMFileSystem::~DDMFileSystem()
{
	for (auto& file : m_WriteFiles)
	{
		if (file.second.is_open())
		{
			file.second.close();
		}
	}
}

bool DDM::DDMFileSystem::OpenWrite(const std::string& fileName)
{
	if (m_WriteFiles.contains(fileName) && m_WriteFiles[fileName].is_open())
	{
		return true;
	}

	CreateDirectory(fileName);

	m_WriteFiles[fileName] = std::ofstream();
	m_WriteFiles[fileName].open(fileName.c_str(), std::ios::binary | std::ios::out);

	return m_WriteFiles[fileName].is_open();
}

bool DDM::DDMFileSystem::CloseWrite(const std::string& fileName)
{
	if (!m_WriteFiles.contains(fileName) || !m_WriteFiles[fileName].is_open())
	{
		return false;
	}

	m_WriteFiles[fileName].close();

	m_WriteFiles.erase(fileName);

	return true;
}

bool DDM::DDMFileSystem::Write(const std::string& fileName, const char* start, size_t size)
{
	if (!m_WriteFiles.contains(fileName) || !m_WriteFiles[fileName].is_open())
	{
		if (!OpenWrite(fileName))
		{
			return false;
		}
	}

	m_WriteFiles[fileName].write(start, size);

	return true;
}

bool DDM::DDMFileSystem::OpenRead(const std::string& fileName)
{
	if (m_WriteFiles.contains(fileName) || (m_ReadFiles.contains(fileName) && m_ReadFiles[fileName].is_open()))
	{
		return false;
	}

	if (!std::filesystem::exists(fileName))
	{
		return false;
	}

	m_ReadFiles[fileName] = std::ifstream();
	m_ReadFiles[fileName].open(fileName, std::ios::binary);

	return true;
}

bool DDM::DDMFileSystem::CloseRead(const std::string& fileName)
{
	if (!m_ReadFiles.contains(fileName) || !m_ReadFiles[fileName].is_open())
	{
		return false;
	}

	m_ReadFiles[fileName].close();

	m_ReadFiles.erase(fileName);

	return true;
}

bool DDM::DDMFileSystem::Read(const std::string& fileName, char* start, size_t size)
{
	if (!m_ReadFiles.contains(fileName) || !m_ReadFiles[fileName].is_open())
	{
		if (!OpenRead(fileName))
		{
			return false;
		}
	}

	m_ReadFiles[fileName].read(start, size);

	return true;
}

void DDM::DDMFileSystem::CreateDirectory(const std::string& filePath)
{
	auto index = filePath.find_last_of("/");

	if (index < 0 || index >= filePath.length())
	{
		std::filesystem::create_directory(filePath);
	}
	else
	{
		auto directoryName = filePath.substr(0, index);

		CreateDirectory(directoryName);

		std::filesystem::create_directory(filePath);
	}

	
}
