#include <iostream>
#include <cstring>

#include "sit-operations.h"

extern const boost::filesystem::path repo_dir(".sit");
extern const boost::filesystem::path stage_dir(".sit/stage");
extern const boost::filesystem::path files_dir(".sit/files");
extern const boost::filesystem::path objects_dir(".sit/objects");
extern boost::uuids::uuid HEAD = boost::uuids::nil_uuid();

int main(int argc, char *const argv[])
{
	if (argc == 1) {
		/*输出版本信息及帮助*/
		return 0;
	}
	if (strcmp(argv[1], "init") == 0) {
		MainFrame::InitRepo();
	}
	if (!boost::filesystem::is_directory(repo_dir)) {
		std::cerr << "sit: error: Not a sit repository" << std::endl;
		exit(0);
	} else {
		Util::GetHead();
	}
	if (strcmp(argv[1], "add") == 0) {
		if (argc == 2) {
			std::cerr << "Need a file name" << std::endl;
			return 0;
		}
		MainFrame::AddFile(argv[2]);
	}
	return 0;
}