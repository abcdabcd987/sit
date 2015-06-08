#ifndef _SRC_REFS_HPP
#define _SRC_REFS_HPP

#include <string>
#include <map>

namespace Sit {
namespace Refs {

using BranchList = std::map<std::string, std::string>;
extern BranchList branches;
extern std::string whichBranch;

void LoadLocalBranch();
void LoadLocalRefs();
void NewBranch(const std::string &branchName, const std::string &id);
void DelBranch(const std::string &branchName);

/**
 * `name` => `refs/heads/name`
 */
std::string Local(const std::string& name);

/**
 * Get the sha1 value of ref path.
 * Throw if not exist.
 */
std::string Get(const std::string& path);

/**
 * Set the sha1 value of ref path.
 */
void Set(const std::string& ref, const std::string& id);

}
}

#endif