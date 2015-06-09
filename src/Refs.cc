#include "Refs.hpp"
#include "Commit.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include <boost/algorithm/string.hpp>

namespace Sit {
namespace Refs {

BranchList branches;
std::string whichBranch;

boost::filesystem::path GetBranchPath(const std::string &ref)
{
	return FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "refs/heads" / ref;
}

void loadLocalBranch()
{
	using namespace FileSystem;
	auto refFileList = ListRecursive(REPO_ROOT / SIT_ROOT / "refs/heads", false, false);
	for (const auto &file : refFileList) {
		if (!IsDirectory(file)) {
			const std::string commitID = Read(file);
			const std::string branchName = GetRelativePath(file).filename().generic_string();
			branches.insert(std::make_pair(branchName, commitID));
		}
	}

	std::string headref = Read(REPO_ROOT / SIT_ROOT / "HEAD");
	if (headref.substr(0, 4) == "ref:") {
		//ref: refs/heads/master
		headref.erase(0, 16);
		boost::trim(headref);
		whichBranch = headref;
	} else {
		branches.insert(std::make_pair("HEAD", headref));
	}
}

void LoadLocalRefs()
{
	loadLocalBranch();
}

std::string Get(const std::string& ref)
{
	if (ref == "HEAD") {
		if (whichBranch.empty()) {
			return branches.at("HEAD");
		} else {
			return branches.at(whichBranch);
		}
	} else {
		if (branches.count(ref) > 0) {
			return branches.at(ref);
		} else {
			throw Util::SitException("Fatal: No such a branch.");
		}
	}
	return Commit::EMPTY_COMMIT;
}

void updateHEAD()
{
	if (branches.count("HEAD") > 0) {
		const std::string headID = branches["HEAD"];
		for (const auto &branch : branches) {
			if (branch.first == "HEAD") {
				continue;
			} else {
				if (branch.second == headID) {
					whichBranch = branch.first;
					branches.erase("HEAD");
					return;
				}
			}
		}
		whichBranch = "";
	}
}
void Set(const std::string& ref, const std::string& id)
{
	if (ref == "HEAD") {
		for (const auto &branch : branches) {
			if (branch.first == "HEAD") {
				continue;
			} else {
				if (branch.second == id) {
					whichBranch = branch.first;
					return;
				}
			}
		}
		branches["HEAD"] = id;
		FileSystem::Write(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "HEAD", id);
		whichBranch = "";
	} else {
		branches[ref] = id;
		FileSystem::Write(GetBranchPath(ref), id);
		updateHEAD();
	}
}

void NewBranch(std::string branchName, const std::string &id)
{
	boost::trim(branchName);
	if (branches.count(branchName) > 0) {
		throw Util::SitException("Fatal: There is a branch with the same name existed.");
	} else {
		if (branchName == "work" || branchName == "index" || branchName == "HEAD") {
			throw Util::SitException("Fatal: " + branchName + " cannot be a branch name.");
		}
	}
	branches.insert(std::make_pair(branchName, id));
}

void DelBranch(const std::string &branchName)
{
	if (branchName != "HEAD" && branches.count(branchName) > 0) {
		branches.erase(branchName);
	} else {
		throw Util::SitException("Fatal: No such a branch.");
	}
}

std::string GetRealID(const std::string &ref)
{
	if (ref.empty()) {
		return ref;
	}
	if (ref == "work" || ref == "index") {
		return ref;
	} else {
		if (branches.count(ref) > 0 || ref == "HEAD") {
			return Get(ref);
		} else {
			return Commit::CommitIDComplete(ref);
		}
	}
}

}
}