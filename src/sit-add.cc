#include "sit-operations.h"

namespace MainFrame {
void AddFile(const boost::filesystem::path &file)
{
	using namespace boost::filesystem;
	if (!is_directory(repo_dir)) {
		std::cerr << "Error: Not a sit repository" << std::endl;
		exit(0);
	}
	try {
		if (!is_directory(file)) {
			Util::safe_copy_file(file, stage_dir / file);
		} else {
			for (recursive_directory_iterator end, cur(file); cur != end; ++cur) {
				if (is_directory(cur->path())) {
					continue;
				}
				Util::safe_copy_file(cur->path(), stage_dir / cur->path());
			}
		}
	} catch (const filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &stdEc) {
		std::cerr << stdEc.what() << std::endl;
	}
#ifdef WIN32
	try {
		if (!is_directory(file)) {
			Util::dos2unix(file);
		} else {
			for (recursive_directory_iterator end, cur(stage_dir / file); cur != end; ++cur) {
				if (is_directory(cur->path())) {
					continue;
				}
				Util::dos2unix(cur->path());
			}
		}
	} catch (const filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &stdEc) {
		std::cerr << stdEc.what() << std::endl;
	}
#endif
}
}