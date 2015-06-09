#include "Core.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "FileSystem.hpp"
#include <iostream>

#include <string>
#include <vector>
#include <sstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;
using std::vector;

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
		cout << "usage: sit add <path1> [<path2> ...]" << endl
		     << endl
		     << desc << endl;
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
	string branchName;
	vector<string> path;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("commit", po::value<string>(&commit)->default_value(""), "Specify the commit which the file will checkout from. Be empty if you want to checkout files in index")
		("branch,b", po::value<string>(&branchName)->default_value(""),  "Create a new branch at the specified commit. Cannot use with path.")
		("path", po::value<vector<string>>(&path), "Path list to checkout. Be empty if you want to checkout the whole commit and update the HEAD to <commit>")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "usage: sit checkout [--commit=<commit-id> | master | HEAD | index] [<path1> <path2> ...]" << endl
			 << endl
			 << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	if (!branchName.empty()) {
		if (vm.count("path")) {
			cout << "Fatal: the option `--branch` cannot be used when any path given." << endl;
			return 1;
		}
	}
	if (!vm.count("path")) {
		path.push_back("");
	}
	for (const auto &p : path) {
		Sit::Core::Checkout(commit, p, branchName);
	}
	return 0;
}

int config(int ac, char** av)
{
	if (ac != 4) {
		cout << "usage: sit config <key> <value>" << endl;
		return ac == 3 && strcmp(av[2], "--help") == 0 ? 0 : 1;
	}
	Sit::Config::Set(av[2], av[3]);
	return 0;
}

int commit(int ac, char** av)
{
	string message;
	po::options_description desc("Allowed options");
	desc.add_options()
		("all,a", "Add all files to index before commit")
		("amend", "Amend history commit")
		("help", "Show this help message")
		("message,m", po::value<string>(&message), "Commit message")
	;
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);
	if (vm.count("help")) {
		cout << "usage: sit commit [-a] [-m <commit messages>]" << endl
		     << "       sit commit --amend [-a] [-m <commit messages>]" << endl
		     << endl
		     << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	if (vm.count("all")) {
		Sit::Core::Add(Sit::FileSystem::REPO_ROOT / ".");
	}
	Sit::Core::Commit(message, vm.count("amend") > 0);
	return 0;
}

int diff(int ac, char **av)
{
	string baseID, targetID;
	vector<string> path;
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Show this help message")
		("base-id", po::value<string>(&baseID)->default_value("index"), "Specify the commit which the one version of files from")
		("target-id", po::value<string>(&targetID)->default_value("work"), "Specify the commit which the anthor version of files from")
		("path", po::value<vector<string>>(&path), "Path list to checkout")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac - 1, av + 1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);
	if (vm.count("help")) {
		cout << "usage: sit diff [--base-id=<baseid>] [--target-id=<targetid>] [<path1> [<path2> ...]]" << endl
		     << endl
		     << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	if (vm.count("path")) {
		Sit::Core::Diff(baseID, targetID, path);
	} else {
		Sit::Core::Diff(baseID, targetID);
	}
	return 0;
}

int help(int argc, char** argv)
{
	cout << "usage: sit <command> [<command arguments>...]" << endl;
#ifdef ENABLE_COMPRESSION
	cout << "objects compression enabled" << endl << endl;
#else
	cout << "objects compression disabled" << endl << endl;
#endif
	cout << "sit commands are:" << endl
	     << "  add        Add file contents to the index" << endl
	     << "  checkout   Checkout paths to the working tree" << endl
	     << "  config     Get and set repository or global options" << endl
	     << "  commit     Record changes to the repository" << endl
	     << "  diff       Show changes between commits, commit and working tree, etc" << endl
	     << "  gc         Remove useless files to free disk space" << endl
	     << "  help       Show this help message" << endl
	     << "  init       Create an empty sit repository or reinitialize an existing one" << endl
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
	/*
	if (ac != 2) {
		cout << "'sit gc' has no extra options" << endl;
		return ac == 3 && strcmp(av[2], "--help") == 0 ? 0 : 1;
	}
	Sit::Core::GarbageCollection();
	*/
	cout << "Warning: Garbage collection is not available now." << endl;
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
	;
	po::positional_options_description p;
	p.add("commit", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "usage: sit log" << endl
		     << "usage: sit log --commit=<commit id>" << endl
		     << endl
		     << desc << endl;
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
		("hard", "Resets the index and working tree, otherwise, only the index but not working tree, cannnot be together with any paths")
		("commit", po::value<string>(&commit)->default_value("HEAD"), "Specify the commit which the index will reset from")
		("path", po::value<vector<string>>(&path), "Path list to reset. If there is no path given, reset will set the current branch head to <commit>")
	;
	po::positional_options_description p;
	p.add("path", -1);
	po::variables_map vm;
	auto parsed = po::command_line_parser(ac-1, av+1).options(desc).positional(p).run();
	po::store(parsed, vm);
	auto unrecog = po::collect_unrecognized(parsed.options, po::include_positional);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "usage: sit reset [--commit=<commit-id> | HEAD | master] <path1> [<path2> ...]" << endl
			 << "       sit reset [--hard] [--commit=<commit-id> | HEAD | master]" << endl 
			 << endl
			 << desc << endl;
		return vm.count("help") ? 0 : 1;
	}
	cout << "The following files have been reset:" << endl << endl;
	if (path.empty()) {
		Sit::Core::Reset(commit, vm.count("hard") > 0);
	} else {
		if (vm.count("hard") > 0) {
			throw Sit::Util::SitException("Fatal: the option \"--hard\" cannot be together with path");
		}
		for (const auto &p : path) {
			Sit::Core::Reset(commit, p);
		}
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
		cout << "usage: sit rm <path1> [<path2> ...]"
		     << desc << endl;
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
			DISPATCHER(help);
			DISPATCHER(init);
			Sit::Core::LoadRepo();

			DISPATCHER(add);
			DISPATCHER(checkout);
			DISPATCHER(config);
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
		std::cerr << e.message << endl;
#ifdef DEBUG
		if (!e.detail.empty()) {
			std::cerr << e.detail << endl;
		}
#endif
	} catch (const boost::filesystem::filesystem_error &fe) {
		std::cerr << fe.what() << endl;
	} catch (const std::exception &ec) {
		std::cerr << ec.what() << endl;
	}
}
