#include "Index.hpp"
#include <iostream>

namespace Sit {
namespace Index {

Index index;

void Index::Load()
{
	try {
		if (FileSystem::IsExist(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "index")) {
			boost::filesystem::ifstream indexFile(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "index");
			unsigned fileCount = 0;
			indexFile >> fileCount;
			indexFile.get();
			for (unsigned i = 0; i < fileCount; ++i) {
				std::string fileName;
				std::string sha1Value;
				indexFile >> sha1Value;
				indexFile.get();
				std::getline(indexFile, fileName);
				_index.insert(std::make_pair(boost::filesystem::path(fileName), sha1Value));
			}
			indexFile.close();
		}
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

void Index::Save() const
{
	try {
		boost::filesystem::ofstream indexFile(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "index");
		indexFile << _index.size() << std::endl;
		for (auto &element : _index) {
			indexFile << element.second << " " << element.first.generic_string() << std::endl;
		}
		indexFile.close();
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

void IndexBase::Insert(const boost::filesystem::path &file, const std::string &content)
{
	try {
		_index[file] = content;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

unsigned IndexBase::Remove(const boost::filesystem::path &path)
{
	unsigned rmCount = 0;
	try {
		auto newPath = path.relative_path();
		std::vector<boost::filesystem::path> wouldRm;
		for (auto &element : _index) {
			if (element.first.string().find(newPath.generic_string()) == 0) {
				wouldRm.push_back(element.first);
				++rmCount;
			}
		}
		for (auto &element : wouldRm) {
			_index.erase(element);
		}
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
	return rmCount;
}

bool IndexBase::InIndex(const boost::filesystem::path& path) const
{
	try {
		return _index.count(path) > 0;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
	return false;
}

const std::map<boost::filesystem::path, std::string>& IndexBase::GetIndex() const
{
	return _index;
}

}
}