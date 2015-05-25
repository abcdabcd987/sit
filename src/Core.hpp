#ifndef _SRC_CORE_HPP_
#define _SRC_CORE_HPP_
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Sit {
namespace Core {

/**
 * Initialize a repository in current working directory
 */
void Init();

/**
 * Try to find a repository directory in the parent directories of current working directory
 */
void LoadRepo();

/**
 * Add files to the index recursively
 */
void Add(const boost::filesystem::path& path);

/**
 * Remove file from working directory and index NOT RECURSIVELY.
 */
void Rm(const boost::filesystem::path& path);

/**
 * Create an commit object and point `HEAD` at that commit.
 * Read commit log from `.sit/COMMIT_MSG`
 */
void Commit(const std::string &msg, const bool isAmend = false);

/**
 * Change the index, working directory and `HEAD` to the commit `id`.
 * Make sure the working directory is clean before action.
 */
void Checkout(std::string commitid, std::string filename);

/**
 * Print the diff of two commits or one commit and current working directory
 */
void Diff(const std::string& base, const std::string& target);

/**
* Print the diff of the same file(filename) between two commits or one commit and current working directory
*/
void Diff(const std::string &baseID, const std::string &targetID, const std::vector<std::string> &filename);

/**
 * Print lists of untracked files, files to be committed, files not staged
 */
void Status();

/**
 * Print commit log
 */
void Log(std::string id);

/**
 * Reset `filename` status in the index to that in the commit `id`
 * If `isHard`, modify the working directory, too.
 * Reset will print message to stream.
 */
void Reset(std::ostream &stream, std::string id, std::string filename);

/**
* Reset `filename` status in the index to that in the commit `id`
* If `isHard`, modify the working directory, too.
* Reset will print message to stream.
*/
void Reset(std::ostream &stream, std::string id, const bool isHard);
/**
 * Delete the objects cannot be reached from the last commit of master in order to free some space in disk.
 */
void GarbageCollection();
}
}
#endif