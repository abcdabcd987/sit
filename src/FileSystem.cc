#include <ios>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "FileSystem.hpp"

namespace Sit {
namespace FileSystem {

const boost::filesystem::path SIT_ROOT(".sit/");
const boost::filesystem::path OBJECTS_DIR(".sit/objects");

boost::filesystem::path REPO_ROOT("./");

bool InRepo()
{
	return boost::filesystem::is_directory(SIT_ROOT);
}

void AssertInRepo()
{
	if (!InRepo()) {
		throw Sit::Util::SitException("Fatal: Not in a sit repo.");
	}
}

std::vector<boost::filesystem::path> ListRecursive(const boost::filesystem::path &path)
{
	using namespace boost::filesystem;
	std::vector<boost::filesystem::path> ls;
	try {
		if (IsDirectory(path)) {
			for (recursive_directory_iterator iter(path), end; iter != end; ++iter) {
				ls.push_back(iter->path().relative_path());
			}
		} else {
			ls.push_back(path);
		}
	} catch (const filesystem_error &fe) {
		throw Sit::Util::SitException(std::string("Fatal: Cannot list path: ") + path.string(), fe.what());
	}
	return ls;
}

void SafeCopyFile(const boost::filesystem::path &from, const boost::filesystem::path &to)
{
	using namespace boost::filesystem;
	if (IsDirectory(from)) {
		return;
	}
	create_directories(to.parent_path());
	copy_file(from, to, copy_option::overwrite_if_exists);
}

bool IsExist(const boost::filesystem::path &path)
{
	return boost::filesystem::exists(path);
}

bool IsFile(const boost::filesystem::path &path)
{
	return boost::filesystem::is_regular_file(path);
}

bool IsDirectory(const boost::filesystem::path &path)
{
	return boost::filesystem::is_directory(path);
}

void Write(const boost::filesystem::path &path, const std::string &content)
{
	using namespace boost::filesystem;
	create_directories(path.parent_path());
	ofstream file(path, std::ios::out | std::ios::binary);
	file.write(content.c_str(), content.length());
	if (!file) {
		throw Util::SitException(std::string("Cannot write to path: ") + path.string());
	}
}

std::string Read(const boost::filesystem::path &path)
{
	using namespace boost::iostreams;
	auto fileSize = boost::filesystem::file_size(path);
	char *s = new char[fileSize];
	boost::filesystem::ifstream file(path, std::ios::in | std::ios::binary);
	file.read(s, fileSize);
	std::string str(s, s + fileSize);
	delete[] s;
	return str;
}

}
}
