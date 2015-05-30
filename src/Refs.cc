#include "Refs.hpp"
#include "FileSystem.hpp"
#include "Util.hpp"
#include <boost/algorithm/string.hpp>

namespace Sit {
namespace Refs {

const std::string EMPTY_REF("0000000000000000000000000000000000000000");


RefMap branchList;
std::string curBranch = "";

void LoadBranch()
{
	auto allBranch = FileSystem::ListRecursive(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "refs/heads");
	for (const auto &branch : allBranch) {
		if (!FileSystem::IsDirectory(branch)) {
			auto branchName = FileSystem::GetRelativePath(branch).generic_string();
			//filename = ".sit/refs/heads/<branch>"
			branchName.erase(0, 16);
			auto content = FileSystem::Read(branch);
			boost::trim(content);
			branchList.insert(std::make_pair(branchName, content));
		}
	}
	auto headContent = FileSystem::Read(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "HEAD");
	boost::trim(headContent);
	//ref: refs/heads/<branch_name>
	//or
	//0000000000000000000000000000000000000000
	if (headContent.substr(0, 4) != "ref:") {
		for (const auto &branch : branchList) {
			if (branch.second == headContent) {
				FileSystem::Write(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "HEAD", "ref: refs/heads/" + branch.first);
				curBranch = branch.first;
				return ;
			}
		}
		branchList.insert(std::make_pair("HEAD", headContent));
	} else {
		curBranch = headContent.substr(16);
	}
}

const std::string &CurBranch()
{
	return curBranch;
}

void SetBranch(const std::string &branchName)
{
	curBranch = branchName;
}

std::string Local(const std::string& name)
{
	return std::string("refs/heads/") + name;
}

std::string Get(const std::string& ref)
{
	if (ref == "HEAD" && !curBranch.empty()) {
		return branchList.at(curBranch);
	}
	if (branchList.count(ref) > 0) {
		return branchList.at(ref);
	}
	throw Util::SitException("Fatal: Bad Ref.", "ref: " + ref);
}

void Set(const std::string& ref, const std::string& id)
{
	if (ref == "HEAD") {
		if (branchList.count(id) > 0) {
			curBranch = id;
		} else {
			for (const auto &branch : branchList) {
				if (branch.second == id) {
					FileSystem::Write(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "HEAD", "ref: refs/heads/" + branch.first);
					curBranch = branch.first;
					return;
				}
			}
			FileSystem::Write(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / "HEAD", id);
		}
	} else {
		if (branchList.count(ref) > 0) {
			branchList[ref] = id;
			const auto path(FileSystem::REPO_ROOT / FileSystem::SIT_ROOT / ref);
			FileSystem::Write(path, id);
		} else {
			throw Util::SitException("Fatal: No such a branch", "ref");
		}
	}
}

}
}