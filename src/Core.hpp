#ifndef _SRC_CORE_HPP_
#define _SRC_CORE_HPP_
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace Sit {
namespace Core {

/**
 * Initializes current working directory
 */
void Init();

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
void Checkout(const std::string& id);

/**
 * Print the diff of two commits or one commit and current working directory
 */
void Diff(const std::string& base, const std::string& target);

/**
 * Print lists of untracked files, files to be committed, files not staged
 */
void Status();

}
}
#endif