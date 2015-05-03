#include <map>
#include <vector>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Core.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include "Index.hpp"
#include "Config.hpp"

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
		create_directories(".sit/objects");
		FileSystem::Write(".sit/HEAD", "0000000000000000000000000000000000000000");
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
		auto fileSize = boost::filesystem::file_size(file);
		if (fileSize > 102400) {
			std::cerr << "Warning : Try to add a file larger than 100MB" << std::endl;
		}
		if (fileSize > 204800) {
			throw Sit::Util::SitException("Fatal: Try to add a file larger than 200MB", file.string());
		}
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

std::string getCommitMessage()
{
	std::stringstream in(FileSystem::Read(FileSystem::SIT_ROOT / "COMMIT_MSG"));
	std::stringstream out;
	std::string line;
	bool empty = true;
	while (getline(in, line)) {
		boost::trim(line);
		if (line.empty()) {
			if (!empty) out << "\n";
		} else if (line[0] != '#') {
			out << line << "\n";
			empty = false;
		}
	}
	return out.str();
}

std::string getHEAD()
{
	auto path(FileSystem::SIT_ROOT / "HEAD");
	if (!FileSystem::IsFile(path)) {
		throw Util::SitException("HEAD not found.");
	}
	boost::filesystem::ifstream in(path);
	std::string str;
	in >> str;
	return str;
}

void Commit()
{
	using Util::SitException;
	using boost::posix_time::to_simple_string;
	using boost::posix_time::second_clock;

	Objects::Commit commit;

	if (!FileSystem::IsFile(FileSystem::SIT_ROOT / "COMMIT_MSG")) {
		throw SitException("Commit message not found.");
	}
	commit.message = getCommitMessage();
	if (commit.message.empty()) {
		throw SitException("Commit message is empty.");
	}
	const std::string user_name = Config::Get("user.name");
	if (user_name == Config::NOT_FOUND) {
		throw SitException("Config `user.name` not found.", "config: user.name");
	}
	const std::string user_email = Config::Get("user.email");
	if (user_email == Config::NOT_FOUND) {
		throw SitException("Config `user.email` not found.", "config: user.email");
	}

	const std::string datetime(to_simple_string(second_clock::local_time()));
	commit.author = Util::AuthorString(user_name, user_email, datetime);
	commit.committer = Util::AuthorString(user_name, user_email, datetime);

	
	commit.parent = getHEAD();
	commit.tree = Objects::WriteIndex();

	const std::string id(Objects::WriteCommit(commit));

	FileSystem::Write(FileSystem::SIT_ROOT / "HEAD", id);
}

}
}