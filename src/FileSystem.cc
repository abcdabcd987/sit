#include <ios>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "FileSystem.hpp"

namespace Sit {
namespace FileSystem {


const boost::filesystem::path SIT_ROOT(".sit/");
const boost::filesystem::path REPO_ROOT("./");


bool InRepo()
{
	return boost::filesystem::is_directory(SIT_ROOT);
}


void AssertInRepo()
{
	if (!InRepo()) {
		throw Sit::Util::SitException("Not in a sit repo.");
	}
}


std::vector<boost::filesystem::path> ListRecursive(const boost::filesystem::path& path)
{
	using namespace boost::filesystem;
	std::vector<boost::filesystem::path> ls;
	try {
		for (recursive_directory_iterator iter(path), end; iter != end; ++iter) {
			ls.push_back(iter->path().relative_path());
		}
	} catch (const filesystem_error &fe) {
		throw Sit::Util::SitException(std::string("Cannot list path: ") + path.string(), fe.what());
	}
	return ls;
}


bool IsExist(const boost::filesystem::path& path)
{
	return boost::filesystem::exists(path);
}


bool IsFile(const boost::filesystem::path& path)
{
	return boost::filesystem::is_regular_file(path);
}


bool IsDirectory(const boost::filesystem::path& path)
{
	return boost::filesystem::is_directory(path);
}


void Write(const boost::filesystem::path& path, const std::string& content)
{
	using namespace boost::filesystem;
	ofstream file(path, std::ios::out | std::ios::binary);
	file << content;
}


std::string Read(const boost::filesystem::path& path)
{
	using namespace boost::iostreams;
	mapped_file mmap(path, boost::iostreams::mapped_file::readonly);
	return mmap.const_data();
}


}
}
