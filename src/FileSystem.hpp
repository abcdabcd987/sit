#ifndef _SRC_FILESYSTEM_HPP_
#define _SRC_FILESYSTEM_HPP_

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
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
* Relative path to the `.sit/objects` directory
*/
extern const boost::filesystem::path OBJECTS_DIR;

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
 * If `path` is a directory, ListRecursive will work regularly.
 * If `path` is a regular file, ListRecursive will return a vector contained only `path`
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
 * Copy a file in a safer way.
 * @param from the file would be copied.
 * @param to   the destination.
 */
void SafeCopyFile(const boost::filesystem::path &from, const boost::filesystem::path &to);

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
