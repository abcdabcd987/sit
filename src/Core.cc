#include <map>
#include <iostream>

#include "Core.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include "Index.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

namespace Sit {
namespace Core {

void Init()
{
	using namespace boost::filesystem;
	try {
		create_directories(".sit/");
#ifdef WIN32
		SetFileAttributes(L".sit", FILE_ATTRIBUTE_HIDDEN);
#endif
		create_directories(".sit/hook");
		create_directories(".sit/objects");
		create_directories(".sit/info");
		create_directories(".sit/log");
		create_directories(".sit/objects");
		create_directories(".sit/objects/info");
		create_directories(".sit/objects/pack");
		create_directories(".sit/refs");
		create_directories(".sit/refs/heads");
		create_directories(".sit/refs/tags");

		ofstream HEAD(".sit/HEAD");
		HEAD << "refs/heads/master" << std::endl;
		HEAD.close();
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &stdEc) {
		std::cerr << stdEc.what() << std::endl;
	}
}

std::string AddFile(const boost::filesystem::path &file)
{
	if (FileSystem::IsDirectory(file)) {
		return "";
	}
	try {
		std::string sha1Value = Sit::Util::SHA1sum(FileSystem::Read(file));
		boost::filesystem::path dstFile(FileSystem::OBJECTS_DIR / sha1Value.substr(0, 2) / sha1Value.substr(2));
		FileSystem::SafeCopyFile(file, dstFile);
		std::cout << file << " added." << std::endl;
		return sha1Value;
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &stdEc) {
		std::cerr << stdEc.what() << std::endl;
	}
	return std::string("");
}

void Add(const boost::filesystem::path &path)
{
	Index::Load();

	auto fileList = FileSystem::ListRecursive(path);
	for (auto &file : fileList) {
		if (FileSystem::IsDirectory(file)) {
			continue;
		}
		Index::Insert(file, AddFile(file));
	}

	Index::Save();
}

void Rm(const boost::filesystem::path &path)
{
	Index::Load();
	std::cout << "Removed " << Index::Remove(path) << " files" << std::endl;
	Index::Save();
}
}
}