#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <algorithm>
#include <functional>

#include "Core.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include "Index.hpp"
#include "Config.hpp"
#include "Refs.hpp"
#include "Status.hpp"
#include "Objects.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

namespace Sit {
namespace Core {

void Init()
{
	using namespace boost::filesystem;
	try {
		if (exists(".sit")) {
			if (is_directory(".sit")) {
				remove_all(".sit");
			} else {
				throw Util::SitException("Fatal: .sit is existed but not a directory please check it.");
			}
		}
		create_directories(".sit");
#ifdef WIN32
		SetFileAttributes(L".sit", FILE_ATTRIBUTE_HIDDEN);
#endif
		create_directories(".sit/refs");
		create_directories(".sit/refs/heads");
		create_directories(".sit/objects");
		FileSystem::Write(".sit/HEAD", Refs::EMPTY_REF);
		FileSystem::Write(".sit/COMMIT_MSG", "");
		FileSystem::Write(".sit/config", "");
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
	throw Sit::Util::SitException("Fatal: Not a sit repository (or any of the parent directories): .sit");
}

std::string addFile(const boost::filesystem::path &file)
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
	auto fileList = FileSystem::ListRecursive(path, true, false);
	for (const auto &file : fileList) {
		if (FileSystem::IsDirectory(file)) {
			continue;
		}
		boost::filesystem::path relativePath = FileSystem::GetRelativePath(file);
		Index::index.Insert(relativePath, addFile(file));
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

void Commit(const std::string &msg, const bool isAmend)
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
	commit.message = msg.empty() ? getCommitMessage() : msg;
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
	Status::PrintStatus(std::cout);
}

void Checkout(std::string commitid, std::string filename)
{
	commitid = Util::SHA1Complete(commitid);
	if (!commitid.empty() && !Objects::IsExist(commitid)) {
		std::cerr << "Error: Commit " << commitid << " doesn't exist." << std::endl;
		return;
	}
	if (commitid == "master") {
		commitid = Refs::Get(Refs::Local("master"));
	} else if (commitid == "HEAD") {
		commitid = Refs::Get("HEAD");
	} else if (commitid == "index") {
		commitid = "";
	}
	if (!filename.empty()) {
		filename = FileSystem::GetRelativePath(filename).generic_string();
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
			const auto src(Objects::GetPath(item.second));
			const auto dst(FileSystem::REPO_ROOT / item.first);
			FileSystem::SafeCopyFile(src, dst);
			Index::index.Insert(item.first, item.second);
		}

		Index::index.Save();
		if (!commitid.empty()) {
			Refs::Set("HEAD", commitid);
		}
	} else {
		// File Checkout

		if (filename.back() != '/' && index.InIndex(filename)) {
			const boost::filesystem::path path(filename);
			const std::string objpath(idx.find(path)->second);
			const auto src(Objects::GetPath(objpath));
			const auto dst(FileSystem::REPO_ROOT / filename);
			FileSystem::SafeCopyFile(src, dst);
		} else {
			const auto &&fileSet(index.ListFile(filename));
			if (!fileSet.empty()) {
				for (const auto &singleFile : fileSet) {
					const auto src(Objects::GetPath(singleFile.second));
					const auto dst(FileSystem::REPO_ROOT / singleFile.first);
					FileSystem::SafeCopyFile(src, dst);
				}
			} else {
				std::cerr << "Error: " << filename << " doesn't exist in file list";
				return;
			}
		}
	}
}

void CheckoutObjects(const std::string &id, const std::string &filename)
{
	const auto src(Objects::GetPath(id));
	const auto dst(FileSystem::REPO_ROOT / filename);
	FileSystem::SafeCopyFile(src, dst);
}

void printLog(std::ostream &out, const Objects::Commit &commit, const std::string &id)
{
	out << Color::BROWN << "Commit " << id << Color::RESET << std::endl
	    << "Author: " << commit.author << std::endl
	    << std::endl;
	std::istringstream ss(commit.message);
	std::string line;
	while (std::getline(ss, line)) out << "    " << line << std::endl;
	out << std::endl;
}

void Log(std::string id)
{
	if (id == "master") {
		id = Refs::Get(Refs::Local("master"));
		while (id != Refs::EMPTY_REF) {
			Objects::Commit commit(Objects::GetCommit(id));
			printLog(std::cout, commit, id);
			id = commit.parent;
		}
	} else {
		Objects::Commit commit(Objects::GetCommit(id));
		printLog(std::cout, commit, id);
	}
}

void resetSingleFile(std::ostream &stream, const std::string &filename, const std::string &objectID, const bool &inCommit, const bool &inIndex, const bool isHard)
{
	
	if (inCommit && !inIndex) {
		stream << "  index <++ ";
		Index::index.Insert(filename, objectID);
		if (isHard) {
			CheckoutObjects(objectID, filename);
		}
	} else if (!inCommit && inIndex) {
		stream << "  index --> ";
		Index::index.Remove(filename);
		if (isHard) {
			FileSystem::Remove(filename);
		}
	} else if (inCommit && inIndex) {
		stream << objectID << " ==> ";
		Index::index.Remove(filename);
		Index::index.Insert(filename, objectID);
		if (isHard) {
			CheckoutObjects(objectID, filename);
		}
	} else {
		std::cerr << "Error: " << filename << " is not tracked" << std::endl;
		return;
	}
	stream << boost::filesystem::path(filename) << std::endl;
	Index::index.Save();
}

void Reset(std::ostream &stream, std::string id, std::string filename)
{
	if (id == "master") {
		id = Refs::Get(Refs::Local("master"));
	} else if (id == "HEAD" || id.empty()) {
		id = Refs::Get("HEAD");
	}

	id = Sit::Util::SHA1Complete(id);
	
	if (!filename.empty()) {
		filename = FileSystem::GetRelativePath(filename).generic_string();
	} else {
		throw Util::SitException("Fatal: there must be some incorrect arguments and a wrong function call happened.");
	}
	
	const Index::CommitIndex commitIndex(id);
	Index::FileSet commitSet = commitIndex.ListFile(filename);
	Index::FileSet indexSet = Index::index.ListFile(filename);
	std::set<std::string> allSet;
	for (const auto &fileInCommit : commitSet) {
		allSet.insert(fileInCommit.first.generic_string());
	}
	for (const auto &fileInIndex : indexSet) {
		allSet.insert(fileInIndex.first.generic_string());
	}
	for (const auto &anyfile : allSet) {
		const bool inCommit = commitSet.count(anyfile) > 0;
		const bool inIndex = indexSet.count(anyfile) > 0;
		resetSingleFile(stream, anyfile, commitIndex.GetID(anyfile), inCommit, inIndex, false);
	}
}

void Reset(std::ostream &stream, std::string id, const bool isHard)
{
	if (id == "master") {
		id = Refs::Get(Refs::Local("master"));
	} else if (id == "HEAD" || id.empty()) {
		id = Refs::Get("HEAD");
	}

	id = Sit::Util::SHA1Complete(id);

	const Index::CommitIndex commitIndex(id);
	const auto &commitSet = commitIndex.GetIndex();
	const auto &indexSet = Index::index.GetIndex();
	std::map<std::string, int> allSet;
	for (const auto &fileInCommit : commitSet) {
		allSet.insert(std::make_pair(fileInCommit.first.generic_string(), 1));
	}
	for (const auto &fileInIndex : indexSet) {
		--allSet[fileInIndex.first.generic_string()];
	}
	for (const auto &anyfile : allSet) {
		bool inCommit;
		bool inIndex;
		if (anyfile.second == 0) {
			inCommit = inIndex = true;
		} else if (anyfile.second == 1) {
			inCommit = true, inIndex = false;
		} else if (anyfile.second == -1) {
			inCommit = false, inIndex = true;
		}
		resetSingleFile(stream, anyfile.first, commitIndex.GetID(anyfile.first), inCommit, inIndex, isHard);
	}
	Refs::Set(Refs::Local("master"), id);
}

void Diff(const std::string &baseID, const std::string &targetID)
{
	Diff::DiffIndex(std::cout, Util::SHA1Complete(baseID), Util::SHA1Complete(targetID));
}

void Diff(const std::string &baseID, const std::string &targetID, const std::vector<std::string> &fileList)
{
	const auto cBaseID = Util::SHA1Complete(baseID);
	const auto cTargetID = Util::SHA1Complete(targetID);
	const Index::IndexBase base(Index::GetIndex(cBaseID));
	const Index::IndexBase target(Index::GetIndex(cTargetID));
	const Diff::DiffList diff(Diff::Diff(base, target));
	Index::IndexList baseFileList;
	Index::IndexList targetFileList;
	for (const auto &file : fileList) {
		const auto newPath = FileSystem::GetRelativePath(file);
		const auto extendedBaseFile = base.ListFile(newPath.generic_string());
		for (const auto &item : extendedBaseFile) {
			baseFileList.push_back(item);
		}
		const auto extendedTargetFile = target.ListFile(newPath.generic_string());
		for (const auto &item : extendedTargetFile) {
			targetFileList.push_back(item);
		}
	}
	std::set<std::string> AllFile;
	for (const auto &file : baseFileList) {
		AllFile.insert(file.first.generic_string());
	}
	for (const auto &file : targetFileList) {
		AllFile.insert(file.first.generic_string());
	}
	for (const auto &file : AllFile) {
		const auto &item = diff.at(file);
		std::cout << "Diffing at " << file << std::endl;
		if (item.status == Diff::Same) {
			std::cout << "  No difference between two version" << std::endl;
		} else {
			Diff::DiffObject(std::cout, item, baseID, targetID);
		}
	}
}

void GarbageCollection()
{
	auto existedList = Objects::ListExistedObjects();
	auto refereddList = Objects::ListReferedObjects();
	for (const auto &element : existedList) {
		if (refereddList.count(element)) {
			continue;
		} else {
			Objects::Remove(element);
		}
	}
}
}
}