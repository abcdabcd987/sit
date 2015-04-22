#include "sit-operations.h"

namespace MainFrame {
void AddFile(const boost::filesystem::path &file)
{
	if (!boost::filesystem::is_directory(repo_dir)) {
		std::cerr << "Error: Not a sit repository" << std::endl;
		exit(0);
	}
	try {
		Util::safe_copy_file(file, repo_dir / "stage" / file);
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << "Failed to copy " << file << " to stage" << std::endl;
		std::cerr << ec.what() << std::endl;
	}
#ifdef WIN32
	try {
		Util::dos2unix(repo_dir / "stage" / file);
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << "Failed to replace CRLF to LF at " << file << std::endl;
		std::cerr << ec.what() << std::endl;
	}
#endif
}
}