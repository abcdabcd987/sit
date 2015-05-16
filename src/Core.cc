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
#include "Refs.hpp"
#include "Status.hpp"

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
		create_directories(".sit/refs");
		create_directories(".sit/refs/heads");
		create_directories(".sit/objects");
		FileSystem::Write(".sit/HEAD", Refs::EMPTY_REF);
		FileSystem::Write(".sit/refs/heads/master", Refs::EMPTY_REF);
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &stdEc) {
		std::cerr << stdEc.what() << std::endl;
	}
}

void LoadRepo()
{
	using namespace boost::filesystem;
	path curPath = current_path();
	while (!curPath.empty()) {
		if (is_directory(curPath / ".sit")) {
			FileSystem::REPO_ROOT = curPath;
			Index::index.Load();
			return ;
		}
		curPath = curPath.parent_path();
	}
	throw Sit::Util::SitException("Cannot find a sit repository.");
}

std::string AddFile(const boost::filesystem::path &file)
{
	if (FileSystem::IsDirectory(file)) {
		return "";
	}
	try {
		auto fileSize = boost::filesystem::file_size(file);
		if (fileSize > (100 << 20)) {
			std::cerr << "Warning : Try to add a file larger than 100MB" << std::endl;
		}
		if (fileSize > (200 << 20)) {
			throw Sit::Util::SitException("Fatal: Try to add a file larger than 200MB", file.string());
		}
		std::string sha1Value = Sit::Util::SHA1sum(FileSystem::Read(file));
		boost::filesystem::path dstFile(FileSystem::REPO_ROOT / FileSystem::OBJECTS_DIR / sha1Value.substr(0, 2) / sha1Value.substr(2));
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
	auto fileList = FileSystem::ListRecursive(path);
	for (auto &file : fileList) {
		if (FileSystem::IsDirectory(file)) {
			continue;
		}
		boost::filesystem::path relativePath = FileSystem::GetRelativePath(file);
		Index::index.Insert(relativePath, AddFile(file));
	}

	Index::index.Save();
}

void Rm(const boost::filesystem::path &path)
{
	Index::index.Remove(FileSystem::GetRelativePath(path));
	Index::index.Save();
}

std::string getCommitMessage()
{
	std::stringstream in(FileSystem::Read(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "COMMIT_MSG"));
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

void amend(const std::string &oldid, const std::string &newid)
{
	std::vector<std::pair<std::string, Objects::Commit>> olds;
	for (std::string id(Refs::Get(Refs::Local("master"))); id != oldid; ) {
		const Objects::Commit commit(Objects::GetCommit(id));
		olds.push_back(std::make_pair(id, commit));
		id = commit.parent;
	}

	std::string last = newid;
	for (auto iter = olds.rbegin(); iter != olds.rend(); ++iter) {
		iter->second.parent = last;
		last = Objects::WriteCommit(iter->second);
	}

	Refs::Set(Refs::Local("master"), last);
}

void Commit(const bool isAmend)
{
	using Util::SitException;
	using boost::posix_time::to_simple_string;
	using boost::posix_time::second_clock;

	const std::string headref(Refs::Get("HEAD"));
	const std::string masterref(Refs::Get(Refs::Local("master")));

	Objects::Commit commit;

	if (headref != masterref && !isAmend) {
		throw SitException("HEAD is not up-to-date with master. Cannot commit.");
	}
	if (!FileSystem::IsFile(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "COMMIT_MSG")) {
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

	if (!isAmend) {
		commit.parent = masterref;
	} else {
		const Objects::Commit oldcommit(Objects::GetCommit(headref));
		commit.parent = oldcommit.parent;
	}
	commit.tree = Objects::WriteIndex();

	const std::string id(Objects::WriteCommit(commit));

	if (!isAmend) {
		Refs::Set(Refs::Local("master"), id);
	} else {
		amend(headref, id);
	}
	Refs::Set("HEAD", id);
}

void Status()
{
	std::cout << Status::StatusString();
}

void Checkout(std::string commitid, const std::string &filename)
{
	commitid = Util::SHA1Complete(commitid);
	if (!commitid.empty() && !Objects::IsExist(commitid)) {
		std::cerr << "Error: Commit " << commitid << " doesn't exist." << std::endl;
		return;
	}
	Index::IndexBase index;
	if (commitid.empty()) {
		index = Index::index;
	} else {
		index = Index::CommitIndex(commitid);
	}
	const std::map<boost::filesystem::path, std::string> &idx(index.GetIndex());

	if (filename.empty()) {
		// Commit Checkout

		if (!Status::IsClean()) {
			std::cerr << "Error: You have something staged. Commit or reset before checkout." << std::endl;
			return;
		}

		Index::index.Clear();
		for (const auto &item : idx) {
			const auto &src(Objects::GetPath(item.second));
			const auto &dst(FileSystem::REPO_ROOT / item.first);
			FileSystem::SafeCopyFile(src, dst);
			Index::index.Insert(item.first, item.second);
		}
		Index::index.Save();		
		Refs::Set("HEAD", commitid);		
	} else {
		// File Checkout

		if (!index.InIndex(filename)) {
			std::cerr << "Error: " << filename << " doesn't exist in file list";
			return;
		}
		const boost::filesystem::path path(filename);
		const std::string objpath(idx.find(path)->second);
		const auto &src(Objects::GetPath(objpath));
		const auto &dst(FileSystem::REPO_ROOT / filename);
		FileSystem::SafeCopyFile(src, dst);
	}
}

void printLog(const Objects::Commit &commit, const std::string &id)
{
	std::cout << "Commit " << id << std::endl
	          << "Author: " << commit.author << std::endl
	          << std::endl
	          << commit.message
	          << std::endl;
}

void Log(std::string id)
{
	if (id == "master") {
		id = Refs::Get(Refs::Local("master"));
		while (id != Refs::EMPTY_REF) {
			Objects::Commit commit(Objects::GetCommit(id));
			printLog(commit, id);
			id = commit.parent;
		}
	} else {
		Objects::Commit commit(Objects::GetCommit(id));
		printLog(commit, id);
	}
}

void Reset(std::string id, const std::string &filename, const bool isHard)
{
	if (id == "master") {
		id = Refs::Get(Refs::Local("master"));
	} else if (id == "HEAD" || id.empty()) {
		id = Refs::Get("HEAD");
	}
	id = Sit::Util::SHA1Complete(id);
	const Index::CommitIndex &commitIndex(id);
	const bool inCommit = commitIndex.InIndex(filename);
	const bool inIndex = Index::index.InIndex(filename);

	if (inCommit && !inIndex) {
		Index::index.Insert(filename, commitIndex.GetID(filename));
		if (isHard) {
			Checkout(id, filename);
		}
	} else if (!inCommit && inIndex) {
		Index::index.Remove(filename);
		if (isHard) {
			FileSystem::Remove(filename);
		}
	} else if (inCommit && inIndex) {
		Index::index.Remove(filename);
		Index::index.Insert(filename, commitIndex.GetID(filename));
		if (isHard) {
			Checkout(id, filename);
		}
	} else {
		std::cerr << "Error: " << filename << " is not tracked" << std::endl;
		return;
	}
	Index::index.Save();
}

void Diff(const std::string &baseID, const std::string &targetID)
{
	std::cout << Diff::DiffIndex(Util::SHA1Complete(baseID), Util::SHA1Complete(targetID));
}

}
}