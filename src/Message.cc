#include "Messages.hpp"

namespace Sit {
namespace Msg {

HelpMessage::HelpMessage() : name(""), description(""), usage("") { }
HelpMessage::HelpMessage(const std::string &_name, const std::string &_description, const std::string &_usage)
	: name(_name), description(_description), usage(_usage) { }

void HelpMessage::Show()
{
	this->ShowAll();
}

void HelpMessage::ShowAll()
{
	std::cout <<
		"Name:\n" << this->name <<
		"\nDescription:\n" << this->description <<
		"\nUsage:\n" << this->usage << std::endl;
}

void HelpMessage::ShowUsage()
{
	std::cout <<
		"Usage:\n" << this->usage << std::endl;
}

AllHelp::AllHelp()
{
	using std::make_pair;
	data["all"] = HelpMessage(
		"    sit is a simplified Git",
		"    A local code repository.",
		"    sit [--help] <command> [<args>]\n\n\n"
		"    The most commonly used sit commands are:\n"
		"       add        Add file contents to the index\n"
		"       checkout   Checkout a branch or paths to the working tree\n"
		"       commit     Record changes to the repository\n"
		"       diff       Show changes between commits, commit and working tree, etc\n"
		"       init       Create an empty Sit repository or reinitialize an existing one\n"
		"       log        Show commit logs\n"
		"       reset      Reset current HEAD to the specified state\n"
		"       rm         Remove files from the working tree and from the index\n"
		"       status     Show the working tree status\n"
		"       help       Show help message\n"
		);
	data["add"] = HelpMessage(
		"    sit-add",
		"    Add file contents to the index\n"
		"    This command updates the index using the current content found in the working tree, to prepare the content staged for the next commit.It typically adds the current content of existing paths as a whole, but with some options it can also be used to add content with only part of the changes made to the working tree files applied, or remove paths that do not exist in the working tree anymore.\n"
		"    This command can be performed multiple times before a commit.It only adds the content of the specified file(s) at the time the add command is run; if you want subsequent changes included in the next commit, then you must run sit add again to add the new content to the index.",
		"    sit add [<files>]"
		);
	data["rm"] = HelpMessage(
		"    sit-rm",
		"    Remove files from the index, or from the working tree and the index.",
		"    sit rm [<files>]"
		);
	data["commit"] = HelpMessage(
		"    sit-commit",
		"    Record changes to the repository\n"
		"    Stores the current contents of the index in a new commit along with a log message from the user describing the changes.",
		"    sit commit [-m <COMMIT_MSG>]"
		);
	data["checkout"] = HelpMessage(
		"    sit-checkout",
		"    Checkout a branch or paths to the working tree\n"
		"    Updates files in the working tree to match the version in the index or the specified tree.",
		"    sit checkout <commit>                         load all files of the commit to current working directory\n"
		"    sit checkout -- <file> [<file> ...]           load some files from HEAD to current working directory\n"
		"    sit checkout <commit> -- <file> [<file> ...]  load some files from the commit to current working directory\n"
		"    \"--\" cannot be ignored if you want to checkout some certain files."
		);
	data["reset"] = HelpMessage(
		"    sit-reset",
		"    Reset current HEAD to the specified state\n",
		"    sit reset [--hard] <commit>\n"
		"    sit reset [--hard] -- <file> [<file> ...]\n"
		"    sit reset [--hard] <commit> -- <file> [<file> ...]\n"
		"    The option \"--hard\" resets the index and working tree. Any changes to tracked files in the working tree since <commit> are discarded."
		);
	data["log"] = HelpMessage(
		"    sit-log",
		"    Show commit logs",
		"    sit log\n"
		"    sit log master\n"
		"    sit log <Commit ID>\n"
		);
	data["diff"] = HelpMessage(
		"    sit-diff",
		"    Show changes between the working tree and the index or a tree, changes between the index and a tree, changes between two trees, changes between two blob objects, or changes between two files on disk.\n",
		"    sit diff [<base>] [<target>]\n"
		"    <base> and <target> could be:\n"
		"        index\n"
		"        work\n"
		"        master\n"
		"        HEAD\n"
		"        <CommitID>\n"
		"    Default <base>   = index\n"
		"    Default <target> = work"
		);
}

void AllHelp::ShowHelp(const std::string &content)
{
	if (data.count(content)) {
		data[content].Show();
	} else {
		throw Util::SitException("Fatal: no such a command.");
	}
}

void AllHelp::ShowUsage(const std::string &content)
{
	if (data.count(content)) {
		data[content].ShowUsage();
	} else {
		throw Util::SitException("Fatal: no such a command.");
	}
}

}
}