#include "Commit.hpp"
#include "Refs.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"

#include <sstream>
#include <boost/algorithm/string.hpp>

namespace Sit {
namespace Commit {

boost::uuids::random_generator generator;
const std::string EMPTY_COMMIT(boost::uuids::to_string(boost::uuids::nil_uuid()));
const boost::filesystem::path COMMIT_DIR("commits");

boost::filesystem::path GetPath(std::string commitID)
{
	return FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / COMMIT_DIR / commitID.substr(0, 4) / commitID.substr(4);
}

bool IsExist(std::string commitID)
{
	//00000000-0000-0000-0000-000000000000
	return FileSystem::IsExist(GetPath(commitID));
}

std::string NewCommitID()
{
	std::string newID;
	do {
		newID = boost::uuids::to_string(generator());
	} while (IsExist(newID));
	return newID;
}

void WriteCommit(const Commit &commit)
{
	std::ostringstream oss;
	oss << "pred";
	for (const auto &p : commit.pred) {
		oss << " " << p;
	}
	oss << "\n"
		<< "succ";
	for (const auto &succ : commit.succ) {
		oss << " " << succ;
	}
	oss << "\n"
		<< "tree " << commit.tree << "\n"
		<< "author " << commit.author << "\n"
		<< "committer " << commit.author << "\n"
		<< "\n"
		<< commit.message;
	FileSystem::CompressWrite(GetPath(commit.selfID), oss.str());
}

Commit ReadCommit(const std::string &commitID)
{
	Commit commit;
	commit.selfID = commitID;
	std::istringstream iss(FileSystem::DecompressRead(GetPath(commitID)));
	//pred 00000000-0000-0000-0000-000000000000 00000000-0000-0000-0000-000000000000
	std::string parentMsg;
	std::getline(iss, parentMsg);
	boost::trim(parentMsg);
	parentMsg.erase(0, 5);
	boost::split(commit.pred, parentMsg, boost::is_any_of(" "));
	//succ 00000000-0000-0000-0000-000000000000 00000000-0000-0000-0000-000000000000
	std::string childMsg;
	std::getline(iss, childMsg);
	boost::trim(childMsg);
	childMsg.erase(0, 5);
	boost::split(commit.succ, childMsg, boost::is_any_of(" "));

	//tree 0000000000000000000000000000000000000000
	std::getline(iss, commit.tree);
	commit.tree.erase(0, 5);
	std::string line;

	// author Scott Chacon <schacon@gmail.com> 1243040974 -0700
	std::getline(iss, commit.author);
	commit.author.erase(0, 7);

	// committer Scott Chacon <schacon@gmail.com> 1243040974 -0700
	std::getline(iss, commit.committer);
	commit.committer.erase(0, 10);

	// Blank Line
	std::getline(iss, line);

	// Message
	while (std::getline(iss, line)) {
		commit.message += line;
		commit.message += '\n';
	}

	return commit;
}

std::string CommitIDComplete(const std::string &_id)
{
	if (_id.length() < 4) {
		throw Util::SitException("Fatal: the commit ID is too short. Please input 4 letters at letters.");
	}
	std::string result;
	using namespace FileSystem;
	auto commitList = ListRecursive(REPO_ROOT / SIT_ROOT / COMMIT_DIR / _id.substr(0, 4), false, true);
	for (const auto &item : commitList) {
		auto commitID = item.generic_string();
		//.sit/commits/0000/0000-0000-0000-0000-000000000000
		commitID.erase(0, 13); 
		commitID.erase(4, 1);
		if (commitID.find(_id) == 0) {
			if (result.empty()) {
				result = commitID;
			} else {
				throw Util::SitException("Fatal: There are several matches. Please input more letters.");
			}
		}
	}
	if (result.empty()) {
		throw Util::SitException("Fatal: There is no match for the given commit-id.");
	}
	return result;
}

void Remove(const std::string &id)
{
	boost::filesystem::remove(GetPath(id));
}

}
}