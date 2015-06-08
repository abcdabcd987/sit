#include "Commit.hpp"
#include "Refs.hpp"

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
	} while (!IsExist(newID));
	return newID;
}

void WriteCommit(const Commit &commit)
{
	std::ostringstream oss;
	oss << "parent";
	for (const auto &p : commit.parent) {
		oss << " " << p;
	}
	oss << "\n"
		<< "child";
	for (const auto &child : commit.child) {
		oss << " " << child;
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
	//parent 00000000-0000-0000-0000-000000000000 00000000-0000-0000-0000-000000000000
	std::string parentMsg;
	std::getline(iss, parentMsg);
	boost::trim(parentMsg);
	parentMsg.erase(0, 7);
	boost::split(commit.parent, parentMsg, boost::is_any_of(' '));
	//child 00000000-0000-0000-0000-000000000000 00000000-0000-0000-0000-000000000000
	std::string childMsg;
	std::getline(iss, childMsg);
	boost::trim(childMsg);
	childMsg.erase(0, 6);
	boost::split(commit.child, childMsg, boost::is_any_of(' '));

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

}
}