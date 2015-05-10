#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include <iostream>
#include <string>

void printCheckoutArg()
{
	std::cout << "Wrong arguments" << std::endl
			  << std::endl
			  << "    sit checkout <commit>" << std::endl
			  << "    sit checkout -- <file>" << std::endl
			  << "    sit checkout <commit> -- <file>" << std::endl
			  << std::endl;
}
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
		} else if (strcmp("status", argv[1]) == 0) {
			Sit::Core::Status();
		} else if (strcmp("checkout", argv[1]) == 0) {
			if (argc == 3) {
				// checkout <commit>
				Sit::Core::Checkout(argv[2], "");
			} else if (argc == 4 && strcmp("--", argv[2]) == 0) {
				// checkout -- <file>
				Sit::Core::Checkout("", argv[3]);
			} else if (argc == 5 && strcmp("--", argv[3]) == 0) {
				// checkout <commit> -- <file>
				Sit::Core::Checkout(argv[2], argv[4]);
			} else {
				printCheckoutArg();
				return 1;
			}
		}
	} catch (const Sit::Util::SitException& e) {
		std::cerr << "!!! Sit Exception:" << std::endl;
		std::cerr << e.message << std::endl;
		if (!e.detail.empty()) {
			std::cerr << e.detail << std::endl;
		}
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << std::endl;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << std::endl;
	}
}
