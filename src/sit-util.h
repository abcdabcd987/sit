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
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

#include <string>

#ifdef _DEBUG
#include <cstdlib>
#endif

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

inline void safe_copy(
	const boost::filesystem::path &from,
	const boost::filesystem::path &to)
{
#ifdef _DEBUG
	std::cerr << "Try to Copy " << from << " To " << to << std::endl;
	system("pause");
#endif
	boost::filesystem::create_directories(to.parent_path());
#ifdef _DEBUG
	std::cerr << "Try to Remove " << to << std::endl;
	system("pause");
#endif
	auto removeCnt = boost::filesystem::remove_all(to);
#ifdef _DEBUG
	std::cerr << "Removed " << removeCnt << " files" << std::endl;
	system("pause");
#endif
	if (!boost::filesystem::is_directory(from)) {
#ifdef _DEBUG
		std::cerr << "Try to Copy a file" << std::endl;
		system("pause");
#endif
		boost::filesystem::copy(from, to);
	} else {
#ifdef _DEBUG
		std::cerr << "Try to Copy a directory" << std::endl;
		system("pause");
#endif
		boost::filesystem::copy_directory(from, to);
	}
}

inline void GetHead()
{
	boost::filesystem::ifstream headIfs(objects_dir / "HEAD", std::ios::binary);
	boost::archive::binary_iarchive headBranchIarch(headIfs);
	headBranchIarch >> curBranch;
	headIfs.close();
	boost::filesystem::ifstream branchIfs(objects_dir / curBranch, std::ios::binary);
	boost::archive::binary_iarchive headUuidIarch(branchIfs);
	branchIfs >> HEAD;
}

inline void SetHead()
{
	boost::filesystem::ofstream headOfs(objects_dir / "HEAD", std::ios::binary);
	boost::archive::binary_oarchive headBranchOarch(headOfs);
	headBranchOarch << curBranch;
	boost::filesystem::ofstream branchOfs(objects_dir / curBranch, std::ios::binary);
	boost::archive::binary_oarchive headUuidOarch(branchOfs);
	headUuidOarch << HEAD;
}

void dos2unix(const boost::filesystem::path &opt_file);

void for_all(const boost::filesystem::path &, void(*opt)(const boost::filesystem::path&));
}

#endif