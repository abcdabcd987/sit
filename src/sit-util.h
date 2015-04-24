#ifndef SIT_UTIL_H_
#define SIT_UTIL_H_

#include <iomanip>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <string>

extern const boost::filesystem::path repo_dir;
extern const boost::filesystem::path stage_dir;
extern const boost::filesystem::path files_dir;
extern const boost::filesystem::path objects_dir;
extern boost::uuids::uuid HEAD;
extern std::string curBranch;

namespace {
const boost::filesystem::directory_iterator endDirIter;
}

namespace Util {
inline void safe_copy_file(
	const boost::filesystem::path &from,
	const boost::filesystem::path &to)
{
	boost::filesystem::create_directories(to.parent_path());
	boost::filesystem::copy_file(from, to, boost::filesystem::copy_option::overwrite_if_exists);
}

inline void GetHead()
{
	boost::filesystem::ifstream headIfs(objects_dir / "HEAD");
	boost::archive::text_iarchive headBranchIarch(headIfs);
	headBranchIarch >> curBranch;
	headIfs.close();
	boost::filesystem::ifstream branchIfs(objects_dir / curBranch);
	boost::archive::text_iarchive headUuidIarch(branchIfs);
	branchIfs >> HEAD;
}

inline void SetHead()
{
	boost::filesystem::ofstream headOfs(objects_dir / "HEAD");
	boost::archive::text_oarchive headBranchOarch(headOfs);
	headBranchOarch << curBranch;
	boost::filesystem::ofstream branchOfs(objects_dir / curBranch);
	boost::archive::text_oarchive headUuidOarch(branchOfs);
	headUuidOarch << HEAD;
}

void dos2unix(const boost::filesystem::path &opt_file);

void for_all(const boost::filesystem::path &, void(*opt)(const boost::filesystem::path&));
}

#endif