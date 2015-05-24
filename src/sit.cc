#include "Core.hpp"
#include "Util.hpp"
#include "FileSystem.hpp"
#include <iostream>

#include <string>
#include <vector>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;
using std::vector;

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
		"    Default <target> = work\n" << endl;
}

int add(int ac, char **av)
{
	vector<string> path;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("path", po::value<vector<string>>(&path), "Path list to add to index")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (!vm.count("path") || vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	for (const auto &p : path) {
		Sit::Core::Add(p);
	}
	return 0;
}

int checkout(int ac, char **av)
{
	string commit;
	vector<string> path;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("commit", po::value<string>(&commit)->default_value(""), "Specify the commit which the file will checkout from")
		("path", po::value<vector<string>>(&path), "Path list to checkout")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (!vm.count("path") || vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	
	for (const auto &p : path) {
		Sit::Core::Checkout(commit, p);
	}
	return 0;
}

int commit(int ac, char** av)
{
	string message;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("message,m", po::value<string>(&message), "Commit message")
		("amend", "Amend history commit")
	;
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	Sit::Core::Commit(message, vm.count("amend"));
	return 0;
}

int diff(int ac, char **av)
{
	// TODO
	if (ac == 2) {
		Sit::Core::Diff("index", "work");
	} else if (ac == 3) {
		Sit::Core::Diff(av[2], "work");
	} else if (ac == 4) {
		Sit::Core::Diff(av[2], av[3]);
	} else {
		printDiffArg();
		return 1;
	}
	return 0;
}

int help(int argc, char** argv)
{
	cout << "usage: sit <command> [<command arguments>...]" << endl
	     << "sit commands are:" << endl
	     << "  add        Add file contents to the index" << endl
	     << "  checkout   Checkout paths to the working tree" << endl
	     << "  commit     Record changes to the repository" << endl
	     << "  diff       Show changes between commits, commit and working tree, etc" << endl
	     << "  gc         Create an empty sit repository or reinitialize an existing one" << endl
	     << "  help       Show this help message" << endl
	     << "  init       Create an empty sit repository" << endl
	     << "  log        Show commit logs" << endl
	     << "  reset      Reset current HEAD to the specified state" << endl
	     << "  rm         Remove files from the working tree and from the index" << endl
	     << "  status     Show the working tree status" << endl
	     << endl
	     << "'sit <command> --help' shows further help information about the command." << endl;
	return 0;
}

int gc(int ac, char** av)
{
	if (ac != 2) {
		cout << "'sit gc' has no extra options" << endl;
		return ac == 3 && strcmp(av[2], "--help") == 0 ? 0 : 1;
	}
	Sit::Core::GarbageCollection();
	return 0;
}

int init(int ac, char** av)
{
	if (ac != 2) {
		cout << "'sit init' has no extra options" << endl;
		return ac == 3 && strcmp(av[2], "--help") == 0 ? 0 : 1;
	}
	Sit::Core::Init();
	return 0;
}

int log(int ac, char** av)
{
	string commit;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("commit", po::value<string>(&commit)->default_value("master"), "Specify log of which commit will show")
		("amend", "Amend history commit")
	;
	po::positional_options_description p;
	p.add("commit", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	Sit::Core::Log(commit);
	return 0;
}

int reset(int ac, char** av)
{
	string commit;
	vector<string> path;
	po::options_description desc("'sit reset' Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("hard", "Resets the index and working tree, otherwise, only the index but not working tree")
		("commit", po::value<string>(&commit)->default_value(""), "Specify the commit which the HEAD will reset to")
		("path", po::value<vector<string>>(&path), "Path list to reset")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (!vm.count("path") || vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	
	for (const auto &p : path) {
		Sit::Core::Reset(commit, p, vm.count("hard"));
	}
	return 0;
}

int rm(int ac, char **av)
{
	vector<string> path;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("path", po::value<vector<string>>(&path), "Path list to remove")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (!vm.count("path") || vm.count("help")) {
		cout << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	for (const auto &p : path) {
		Sit::Core::Rm(p);
	}
	return 0;
}

int status(int ac, char** av)
{
	if (ac != 2) {
		cout << "'sit status' has no extra options" << endl;
		return ac == 3 && strcmp(av[2], "--help") == 0 ? 0 : 1;
	}
	Sit::Core::Status();
	return 0;
}

#define DISPATCHER(COMMAND) \
	do { if (strcmp(argv[1], #COMMAND) == 0) return COMMAND(argc, argv); } while (0)

int main(int argc, char** argv)
{
	try {
		if (argc > 1) {
			DISPATCHER(init);
			Sit::Core::LoadRepo();

			DISPATCHER(add);
			DISPATCHER(checkout);
			DISPATCHER(commit);
			DISPATCHER(diff);
			DISPATCHER(gc);
			DISPATCHER(help);
			DISPATCHER(log);
			DISPATCHER(reset);
			DISPATCHER(rm);
			DISPATCHER(status);
		}
		help(argc, argv);
	} catch (const Sit::Util::SitException& e) {
		std::cerr << "!!! Sit Exception:" << endl;
		std::cerr << e.message << endl;
		if (!e.detail.empty()) {
			std::cerr << e.detail << endl;
		}
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << endl;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << endl;
	}
}
