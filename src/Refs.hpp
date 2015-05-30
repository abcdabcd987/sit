#ifndef _SRC_REFS_HPP
#define _SRC_REFS_HPP

#include <unordered_map>
#include <string>

namespace Sit {
namespace Refs {

extern const std::string EMPTY_REF;

using RefMap = std::unordered_map<std::string, std::string>;
extern RefMap branchList;

/*
 * Load all branches from `.sit/refs/`
 */
void LoadBranch();

/*
 * return which branch `HEAD` is located now.
 */
const std::string &CurBranch();

/*
 * set current branch to `branchName`
 */
void SetBranch(const std::string &branchName);

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