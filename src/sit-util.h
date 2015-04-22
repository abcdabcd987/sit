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

extern const boost::filesystem::path repo_dir;
extern const boost::filesystem::path stage_dir;
extern const boost::filesystem::path files_dir;
extern const boost::filesystem::path objects_dir;
extern boost::uuids::uuid HEAD;

namespace Util {
inline void safe_copy_file(
	const boost::filesystem::path &from,
	const boost::filesystem::path &to)
{
	boost::filesystem::create_directories(to.parent_path());
	boost::filesystem::copy_file(from, to, boost::filesystem::copy_option::overwrite_if_exists);
}

inline void dos2unix(const boost::filesystem::path &opt_file)
{
	auto fileSize = boost::filesystem::file_size(opt_file);
	char *s = new char[fileSize];

	boost::filesystem::ifstream getData(opt_file, std::ios::in | std::ios::binary);
	getData.read(s, fileSize);
	getData.close();
	
	boost::filesystem::ofstream writeData(opt_file, std::ios::out | std::ios::trunc | std::ios::binary);
	for (unsigned i = 0; i < fileSize; ++i) {
		if (i < fileSize - 1 && s[i] == '\r' && s[i + 1] == '\n') {
			continue;
		}
		writeData.write(s + i, 1);
	}
	writeData.close();
	delete[] s;
}

inline void GetHead()
{
	boost::filesystem::ifstream ifs(objects_dir / "HEAD", std::ios::binary);
	boost::archive::binary_iarchive biarch(ifs);
	biarch >> HEAD;
}

inline void SetHead(const boost::uuids::uuid &newHead)
{
	boost::filesystem::ofstream ofs(objects_dir / "HEAD", std::ios::binary);
	boost::archive::binary_oarchive boarch(ofs);
	boarch << HEAD;
}
}

#endif