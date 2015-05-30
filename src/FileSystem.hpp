#ifndef _SRC_FILESYSTEM_HPP_
#define _SRC_FILESYSTEM_HPP_

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>

namespace Sit {
namespace FileSystem {

/**
 * Relative path the the `.sit` root
 */
extern const boost::filesystem::path SIT_ROOT;

/**
 * Relative path to the repo root, i.e., `./`
 */
extern boost::filesystem::path REPO_ROOT;

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
std::vector<boost::filesystem::path> ListRecursive(const boost::filesystem::path &path, const bool ignoreSit = true, const bool relativeToRepoRoot = true);

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

/**
 * Remove file `path`
 */
void Remove(const boost::filesystem::path& path);

/**
 * Get a relative path from base to file.
 */
boost::filesystem::path GetRelativePath(
	const boost::filesystem::path &file,
	const boost::filesystem::path &base = REPO_ROOT);

/**
 * Hash a file
 */
std::string FileSHA1(const boost::filesystem::path &path);

/**
 * Copy from src to dst with gzlib compression
 */
void CompressCopy(const boost::filesystem::path &src, const boost::filesystem::path &dst);

/**
 * Write content to dst with gzlib compression
 */
void CompressWrite(const boost::filesystem::path &dst, const std::string &content);

/**
 * Extract src to dst
 */
void DecompressCopy(const boost::filesystem::path &src, const boost::filesystem::path &dst);

/**
 * Return the extracted src
 */
std::string DecompressRead(const boost::filesystem::path &src);


}
}
#endif
