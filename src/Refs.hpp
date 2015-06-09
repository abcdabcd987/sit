#ifndef _SRC_REFS_HPP
#define _SRC_REFS_HPP

#include <string>
#include <map>

namespace Sit {
namespace Refs {

using BranchList = std::map<std::string, std::string>;
extern BranchList branches;
extern std::string whichBranch;

/**
 * Load all references from the repository when sit starting. Like HEAD, branches, tags.
 */
void LoadLocalRefs();

/**
 * Create a new branch nameed `branchName` at the commit with ID `id`
 */
void NewBranch(std::string branchName, const std::string &id);

/**
 * Delete the branch named `branchName`
 */
void DelBranch(const std::string &branchName);

/**
 * Get the uuid of ref path.
 * Throw if not exist.
 */
std::string Get(const std::string& path);

/**
 * Set the uuid of ref path.
 */
void Set(const std::string& ref, const std::string& id);

/**
 * Return the commit ID of a reference, like HEAD, branch name, a prefix of a commit ID
 */
std::string GetRealID(const std::string &ref);
}
}

#endif