#include <iostream>
#include <cstring>

#include "sit-options.h"

extern const boost::filesystem::path repo_dir(".sit");

int main(int argc, char *const argv[])
{
	if (argc == 1) {
		/*输出版本信息及帮助*/
		return 0;
	}
	if (strcmp(argv[1], "init") == 0) {
		MainFrame::init_repo();
	}
	if (strcmp(argv[1], "add") == 0) {
		if (argc == 2) {
			std::cerr << "Need a file name" << std::endl;
			return 0;
		}
		MainFrame::add_file(argv[2]);
	}
	return 0;
}