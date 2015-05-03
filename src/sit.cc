#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	try {
		if (strcmp("init", argv[1]) == 0) {
			Sit::Core::Init();
		}
		Sit::Core::LoadRepo();
		std::cout << Sit::FileSystem::REPO_ROOT << std::endl;
		if (strcmp("add", argv[1]) == 0) {
			std::cout << argv[2] << std::endl;
			Sit::Core::Add(argv[2]);
		} else if (strcmp("rm", argv[1]) == 0) {
			std::cout << argv[2] << std::endl;
			Sit::Core::Rm(argv[2]);
		} else if (strcmp("commit", argv[1]) == 0) {
			Sit::Core::Commit();
		}
	} catch (const Sit::Util::SitException& e) {
		std::cerr << "!!! Sit Exception:" << std::endl;
		std::cerr << e.message << std::endl;
		if (!e.detail.empty()) {
			std::cerr << e.detail << std::endl;
		}
	}
}
