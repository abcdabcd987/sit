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
void Commit();

/**
 * Change the index, working directory and `HEAD` to the commit `id`.
 * Make sure the working directory is clean before action.
 */
void Checkout(const std::string &commitid, const std::string &filename);

/**
 * Print the diff of two commits or one commit and current working directory
 */
void Diff(const std::string& base, const std::string& target);

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
 */
void Reset(std::string id, const std::string &filename, const bool isHard);

}
}
#endif