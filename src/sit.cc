#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include <iostream>
#include <string>

void printCheckoutArg()
{
	std::cout << "Wrong arguments" << std::endl
			  << "    sit checkout <commit>" << std::endl
			  << "    sit checkout -- <file>" << std::endl
			  << "    sit checkout <commit> -- <file>" << std::endl
			  << std::endl;
}
void printLogArg()
{
	std::cerr << "Wrong arguments" << std::endl
	          << "    sit log" << std::endl
	          << "    sit log master" << std::endl
	          << "    sit log <Commit ID>" << std::endl
	          << std::endl;
}
void printResetArg()
{
	std::cout << "Wrong arguments" << std::endl
			  << "    sit reset [--hard] <commit>" << std::endl
			  << "    sit reset [--hard] -- <file>" << std::endl
			  << "    sit reset [--hard] <commit> -- <file>" << std::endl
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
		} else if (strcmp("log", argv[1]) == 0) {
			if (argc == 2) {
				Sit::Core::Log("master");
			} else if (argc == 3) {
				Sit::Core::Log(argv[2]);
			} else {
				printLogArg();
				return 1;
			}
		} else if (strcmp("reset", argv[1]) == 0) {
			int offset = argc >= 3 && strcmp("--hard", argv[2]) == 0 ? 1 : 0;
			if (argc == offset+3) {
				// reset [--hard] <commit>
				Sit::Core::Reset(argv[offset+2], "", offset);
			} else if (argc == offset+4 && strcmp("--", argv[offset+2]) == 0) {
				// reset [--hard] -- <file>
				Sit::Core::Reset("", argv[offset+3], offset);
			} else if (argc == offset+5 && strcmp("--", argv[offset+3]) == 0) {
				// reset [--hard] <commit> -- <file>
				Sit::Core::Reset(argv[offset+2], argv[offset+4], offset);
			} else {
				printResetArg();
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
