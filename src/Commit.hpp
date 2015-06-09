#ifndef _SRC_COMMIT_HPP
#define _SRC_COMMIT_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <vector>

#include "Objects.hpp"

namespace Sit {
namespace Commit {

using boost::uuids::uuid;

extern const std::string EMPTY_COMMIT;

bool IsExist(std::string commitID);

/**
 * Get commit filesystem path
 */
boost::filesystem::path GetPath(std::string commitID);

/**
 * Use random_generator to generator a new uuid.
 * return a std::string as the new commit ID.
 */
std::string NewCommitID();

/**
 * A structure to save all messages of a commit when sit is running.
 */
struct Commit {
	std::string selfID;
	std::vector<std::string> pred;      // The uuid of the pred commit.
	std::vector<std::string> succ;      // the uuid of the succ commit.
	std::string tree;                   // The sha1 value of the repo root tree
	std::string author;                 // The author's name, email, timestamp, timezone
	std::string committer;              // The committer's name, email, timestamp, timezone
	std::string message;                // Commit log
};

/**
 * Write a commit to filesystem in a certain format.
 * @param commit the commit which will be written to the filesystem.
 */
void WriteCommit(const Commit &commit);

/**
 * Get a commit from repository.
 * @param  commitID the UUID of the commit.
 * @return          a commit structure which contain all the message of the commit.
 */
Commit ReadCommit(const std::string &commitID);

/**
 * Complete a commit ID with a commit in the filesystem.
 * @param  _id a prefix of the commit ID, must be given more then 4 letters.
 * @return     the full id of the commit.
 */
std::string CommitIDComplete(const std::string &_id);

/**
 * remove a commit in the repository.
 */
void Remove(const std::string &id);

}
}



#endif