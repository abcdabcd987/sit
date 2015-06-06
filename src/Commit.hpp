#ifndef _SRC_COMMIT_HPP
#define _SRC_COMMIT_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <vector>

#include "FileSystem.hpp"
#include "Objects.hpp"

namespace Sit {
namespace Commit {

using boost::uuids::uuid;
extern const std::string EMPTY_COMMIT;
bool IsExist(std::string commitID);
boost::filesystem::path GetPath(std::string commitID);
std::string NewCommitID();

struct Commit {
	std::string selfID;
	std::vector<std::string> parent;
	std::vector<std::string> child;
	std::string tree;
	std::string author;
	std::string committer;
	std::string message;
};

void WriteCommit(const Commit &commit);
}
}



#endif