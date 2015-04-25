#ifndef _SRC_FILESYSTEM_HPP_
#define _SRC_FILESYSTEM_HPP_

#include <vector>
#include <boost/filesystem.hpp>
#include "Util.hpp"
#include "Objects.hpp"

namespace Sit {
namespace FileSystem {



/**
 * Relative path the the `.sit` root
 */
extern const boost::filesystem::path SIT_ROOT;


/**
 * Relative path to the repo root, i.e., `./`
 */
extern const boost::filesystem::path REPO_ROOT;


/**
 * Return whether the current working directory is in the repo or not
 */
bool InRepo();


/**
 * Assert the current working directory is in the repo
 */
void AssertInRepo();


/**
 * List all files and directories under the path recursively.
 * Return a vector of the path list, sorted.
 */
std::vector<boost::filesystem::path> ListRecursive(const boost::filesystem::path& path);


///**
// * Return the relative path of the given argument to the path of the repo root
// */
//boost::filesystem::path PathFromRepoRoot(const boost::filesystem::path& path);
//
// Seems not useful??


///**
// * Return the relative path of the given argument to the path of the `.sit` root
// */
//boost::filesystem::path PathFromSitRoot(const boost::filesystem::path& path);
//
// Seems not useful?


/**
 * Check whether the `path` exists
 */
bool IsExist(const boost::filesystem::path& path);


/**
 * Check whether the `path` exists and is a file
 */
bool IsFile(const boost::filesystem::path& path);


/**
 * Check whether the `path` exists and is a directory
 */
bool IsDirectory(const boost::filesystem::path& path);


/**
 * Write `content` to `path`. Override it if exists.
 */
void Write(const boost::filesystem::path& path, const std::string& content);


/**
 * Read file `path` to a string.
 */
std::string Read(const boost::filesystem::path& path);


}
}
#endif
