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
		Util::safe_copy(file, repo_dir / "stage" / file);
	} catch (const filesystem_error &ec) {
		std::cerr << "Failed to copy " << file << " to stage" << std::endl;
		std::cerr << ec.what() << std::endl;
	}
#ifdef WIN32
	Util::for_all(repo_dir / "stage" / file, &Util::dos2unix);
#endif
}
}