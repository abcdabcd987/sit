#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include "Message.hpp"
#include <iostream>

#include <string>
#include <vector>

Sit::Msg::AllHelp allHelp;

void printCheckoutArg()
{
	std::cout << 
		"Wrong arguments\n"
		"    sit checkout <commit>\n"
		"    sit checkout -- <file> [<file> ...]\n"
		"    sit checkout <commit> -- <file> [<file> ...]\n" << std::endl;
}

void printLogArg()
{
	std::cerr << 
		"Wrong arguments\n"
		"    sit log\n"
		"    sit log master\n"
		"    sit log <Commit ID>\n" << std::endl;
}

void printResetArg()
{
	std::cerr << 
		"Wrong arguments\n"
		"    sit reset [--hard] <commit>\n"
		"    sit reset [--hard] -- <file> [<file> ...]\n"
		"    sit reset [--hard] <commit> -- <file> [<file> ...]\n" << std::endl;
}

void printDiffArg()
{
	std::cerr <<
		"Wrong arguments\n"
		"    sit diff [<base>] [<target>]\n"
		"    <base> and <target> could be:\n"
		"        index\n"
		"        work\n"
		"        master\n"
		"        HEAD\n"
		"        <CommitID>\n"
		"    Default <base>   = index\n"
		"    Default <target> = work\n" << std::endl;
}

std::ostringstream COMMIT_MSG_STR;
void WriteCommitMessage()
{
	if (COMMIT_MSG_STR.str().empty()) {
		Sit::FileSystem::Write(Sit::FileSystem::REPO_ROOT / Sit::FileSystem::SIT_ROOT / "COMMIT_MSG", "No Messages");
	} else {
		Sit::FileSystem::Write(Sit::FileSystem::REPO_ROOT / Sit::FileSystem::SIT_ROOT / "COMMIT_MSG", COMMIT_MSG_STR.str());
	}
}

std::vector<std::string> argv;

int main(int argc, char** av)
{
	argv = std::vector<std::string>(av, av + argc);
	try {
		if (argv[1] == "--help" || argv[1] == "help") {
			if (argc == 2) {
				allHelp.ShowHelp("all");
			} else {
				for (int i = 2; i < argc; ++i) {
					allHelp.ShowHelp(argv[i]);
				}
			}
			return 0;
		}
		if (argv[1] == "init") {
			Sit::Core::Init();
			return 0;
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
			if (offset) {
				if (argc > 4 && argv[3] == "-m") {
					for (int i = 4; i < argc; ++i) {
						COMMIT_MSG_STR << argv[i] << " ";
					}
				}
			} else {
				if (argc > 3 && argv[2] == "-m") {
					for (int i = 3; i < argc; ++i) {
						COMMIT_MSG_STR << argv[i] << " ";
					}
				}
			}
			WriteCommitMessage();
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
				for (auto i = offset + 3; i < argc; ++i) {
					Sit::Core::Reset("", argv[i], offset);
				}
			} else if (argc >= offset + 5 && argv[offset + 3] == "--") {
				// reset [--hard] <commit> -- <file>
				for (auto i = offset + 4; i < argc; ++i) {
					Sit::Core::Reset(argv[offset + 2], argv[i], offset);
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
