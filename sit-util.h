#ifndef SIT_UTIL_H_
#define SIT_UTIL_H_

#include <iomanip>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

extern const boost::filesystem::path repo_dir;

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
	auto file_size = boost::filesystem::file_size(opt_file);
	char *s = new char[file_size];

	boost::filesystem::ifstream get_data(opt_file, std::ios::in | std::ios::binary);
	get_data.read(s, file_size);
	get_data.close();
	
	boost::filesystem::ofstream write_data(opt_file, std::ios::out | std::ios::trunc | std::ios::binary);
	for (unsigned i = 0; i < file_size; ++i) {
		if (i < file_size - 1 && s[i] == '\r' && s[i + 1] == '\n') {
			continue;
		}
		write_data.write(s + i, 1);
	}
	write_data.close();
	delete[] s;
}
}

#endif