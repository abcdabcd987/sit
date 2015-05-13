#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include <iostream>

#include <boost/program_options.hpp>
#include <string>
#include <vector>

namespace opt = boost::program_options;

void printCheckoutArg()
{
	std::cout << "Wrong arguments" << std::endl
			  << "    sit checkout <commit>" << std::endl
			  << "    sit checkout -- <file> [<file> ...]" << std::endl
			  << "    sit checkout <commit> -- <file> [<file> ...]" << std::endl
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
			  << "    sit reset [--hard] -- <file> [<file> ...]" << std::endl
			  << "    sit reset [--hard] <commit> -- <file> [<file> ...]" << std::endl
			  << std::endl;
}
void printDiffArg()
{
	std::cout << "Wrong arguments" << std::endl
	          << "    sit diff [<base>] [<target>]" << std::endl
	          << "    <base> and <target> could be:" << std::endl
	          << "        index" << std::endl
	          << "        work" << std::endl
	          << "        master" << std::endl
	          << "        HEAD" << std::endl
	          << "        <CommitID>" << std::endl
	          << "    Default <base>   = index" << std::endl
	          << "    Default <target> = work" << std::endl
	          << std::endl;
}

std::vector<std::string> argv;

int main(int argc, char** av)
{
	argv = std::vector<std::string>(av, av + argc);
	try {
		if (argv[1] == "init") {
			Sit::Core::Init();
		}
		Sit::Core::LoadRepo();
		if (argv[1] == "add") {
			for (auto iter = argv.begin() + 2; iter != argv.end(); ++iter) {
				Sit::Core::Add(*iter);
			}
		} else if (argv[1] == "rm") {
			for (auto iter = argv.begin() + 2; iter != argv.end(); ++iter) {
				Sit::Core::Rm(*iter);
			}
		} else if (argv[1] == "commit") {
			const int offset = argc > 2 && argv[2] == "--amend" ? 1 : 0;
			Sit::Core::Commit(offset);
		} else if (argv[1] == "status") {
			Sit::Core::Status();
		} else if (argv[1] == "checkout") {
			if (argc == 3) {
				// checkout <commit>
				Sit::Core::Checkout(argv[2], "");
			} else if (argc >= 4 && argv[2] == "--") {
				// checkout -- <file>
				for (auto iter = argv.begin() + 3; iter != argv.end(); ++iter) {
					Sit::Core::Checkout("", *iter);
				}
			} else if (argc >= 5 && argv[3] == "--") {
				// checkout <commit> -- <file>
				for (auto iter = argv.begin() + 4; iter != argv.end(); ++iter) {
					Sit::Core::Checkout(argv[2], *iter);
				}
			} else {
				printCheckoutArg();
				return 1;
			}
		} else if (argv[1] == "log") {
			if (argc == 2) {
				Sit::Core::Log("master");
			} else if (argc == 3) {
				Sit::Core::Log(argv[2]);
			} else {
				printLogArg();
				return 1;
			}
		} else if (argv[1] == "reset") {
			int offset = argc >= 3 && argv[2] == "--hard" ? 1 : 0;
			if (argc == offset + 3) {
				// reset [--hard] <commit>
				Sit::Core::Reset(argv[offset + 2], "", offset);
			} else if (argc >= offset + 4 && argv[offset + 2] == "--") {
				// reset [--hard] -- <file>
				for (auto i = 3; i < argc; ++i) {
					Sit::Core::Reset("", argv[offset + i], offset);
				}
			} else if (argc >= offset + 5 && argv[offset + 3] == "--") {
				// reset [--hard] <commit> -- <file>
				for (auto i = 4; i < argc; ++i) {
					Sit::Core::Reset(argv[offset + 2], argv[offset + i], offset);
				}
			} else {
				printResetArg();
				return 1;
			}
		} else if (argv[1] == "diff") {
			if (argc == 2) {
				Sit::Core::Diff("index", "work");
			} else if (argc == 3) {
				Sit::Core::Diff(argv[2], "work");
			} else if (argc == 4) {
				Sit::Core::Diff(argv[2], argv[3]);
			} else {
				printDiffArg();
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
