#include "sit-options.h"

namespace MainFrame {
void init_repo()
{
	try {
		boost::filesystem::create_directories(".sit");
#ifdef WIN32
		SetFileAttributes(L".sit", FILE_ATTRIBUTE_HIDDEN);
#endif
		boost::filesystem::create_directories(".sit/files");
		boost::filesystem::create_directories(".sit/stage");
		boost::filesystem::create_directories(".sit/objects");
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

void add_file(const boost::filesystem::path &file)
{
	if (!boost::filesystem::is_directory(repo_dir)) {
		std::cerr << "Error: Not a sit repository" << std::endl;
		exit(0);
	}
	try {
		util::safe_copy_file(file, repo_dir / "stage" / file);
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << "Failed to copy " << file << " to stage" << std::endl;
		std::cerr << ec.what() << std::endl;
	}
#ifdef WIN32
	try {
		util::dos2unix(repo_dir / "stage" / file);
	} catch (const boost::filesystem::filesystem_error &ec) {
		std::cerr << "Failed to replace CRLF to LF at " << file << std::endl;
		std::cerr << ec.what() << std::endl;
	}
#endif
}
}