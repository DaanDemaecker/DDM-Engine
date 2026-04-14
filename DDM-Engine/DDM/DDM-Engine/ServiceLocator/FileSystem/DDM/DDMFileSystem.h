// DDMFileSystem.h
// This file will hold an implementation of the IFileSystem

#ifndef _DDM_DDM_FILE_SYSTEM_
#define _DDM_DDM_FILE_SYSTEM_

// Parent include
#include "DDM-Engine/ServiceLocator/FileSystem/IFileSystem.h"

// Export include
#include "DDM-Engine/Export.h"

// Standard library includes
#include <fstream>
#include <unordered_map>

namespace DDM
{
	class DDM_API DDMFileSystem final : public IFileSystem
	{
	public:
		DDMFileSystem();
		virtual ~DDMFileSystem();

		DDMFileSystem(DDMFileSystem&) = delete;
		DDMFileSystem(DDMFileSystem&&) = delete;

		DDMFileSystem& operator=(DDMFileSystem&) = delete;
		DDMFileSystem& operator=(DDMFileSystem&&) = delete;

		virtual bool OpenWrite(const std::string& fileName) override;

		virtual bool CloseWrite(const std::string& fileName) override;

		virtual bool Write(const std::string& fileName, const char* start, size_t size) override;

		virtual bool OpenRead(const std::string& fileName) override;

		virtual bool CloseRead(const std::string& fileName) override;

		virtual bool Read(const std::string& fileName, char* start, size_t size) override;
	private:
		std::unordered_map<std::string, std::ofstream> m_WriteFiles{};

		std::unordered_map<std::string, std::ifstream> m_ReadFiles{};

		void CreateDirectory(const std::string& filePath);
	};
}

#endif // !_DDM_DDM_FILE_SYSTEM_
