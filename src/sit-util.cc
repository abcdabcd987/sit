#include "sit-util.h"

namespace Util {
void dos2unix(const boost::filesystem::path &opt_file)
{
	if (boost::filesystem::is_directory(opt_file)) {
		throw boost::filesystem::filesystem_error(
			std::string("Cannot Convert a directory from dos to unix"),
			boost::system::error_code()
			);
	}

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

void for_all(const boost::filesystem::path &opt_file, void (*opt)(const boost::filesystem::path&))
{
	using namespace boost::filesystem;
	if (!is_directory(opt_file)) {
		opt(opt_file);
	} else {
		for (directory_iterator cur(opt_file); cur != endDirIter; ++cur) {
			if (is_directory(cur->path())) {
				for_all(cur->path(), opt);
			} else {
				opt(cur->path());
			}
		}
	}
}
}