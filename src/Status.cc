#include <boost/filesystem.hpp>
#include <vector>
#include <sstream>

#include "Diff.hpp"
#include "Index.hpp"
#include "Status.hpp"
#include "FileSystem.hpp"
#include "Refs.hpp"

#include <iostream>

namespace Sit {
namespace Status {

struct StatusItem {
	boost::filesystem::path path;
	Diff::FileStatus status;
};
typedef std::vector<StatusItem> List;

List getUntracked(const Index::WorkingIndex &workIndex, const Index::Index &index)
{
	List v;
	const auto &work(workIndex.GetIndex());
	for (const auto &item : work) {
		if (!index.InIndex(item.first)) {
			v.push_back(StatusItem({item.first, Diff::Untracked}));
		}
	}
	return v;
}

List getToBeCommitted(const Index::CommitIndex &head, const Index::Index &index)
{
	List v;
	const Diff::DiffList diff(Diff::Diff(head, index));
	for (const auto &item : diff) {
		if (item.second.status != Diff::Same) {
			v.push_back(StatusItem({item.first, item.second.status}));
		}
	}
	return v;
}

List getNotStaged(const Index::Index &index, const Index::WorkingIndex &work)
{
	List v;
	const Diff::DiffList diff(Diff::Diff(index, work));
	for (const auto &item : diff) {
		if (item.second.status != Diff::Same && index.InIndex(item.first)) {
			v.push_back(StatusItem({item.first, item.second.status}));
		}
	}
	return v;
}

void print(std::ostream &out, const List &v)
{
	for (const auto &item : v) {
		out << "        ";
		switch (item.status) {
			case Diff::Added:     out << "New File:   "; break;
			case Diff::Modified:  out << "Modified:   "; break;
			case Diff::Deleted:   out << "Deleted:    "; break;
			case Diff::Same:                             break;
			case Diff::Untracked:                        break;
		}
		out << FileSystem::GetRelativePath(item.path).generic_string() << std::endl;
	}
}

std::string StatusString()
{
	const std::string headref(Refs::Get("HEAD"));
	std::ostringstream ss;
	if (headref == Refs::EMPTY_REF) {
		ss << "The current repo is empty." << std::endl;
		return ss.str();
	}

	const Index::CommitIndex head(headref);
	const Index::WorkingIndex work;
	const Index::Index &index(Index::index);

	List untracked(getUntracked(work, index));
	List toBeCommitted(getToBeCommitted(head, index));
	List notStaged(getNotStaged(index, work));

	ss << "On branch: master" << std::endl;

	if (untracked.empty() && toBeCommitted.empty() && notStaged.empty()) {
		ss << "Nothing to commit, working directory clean" << std::endl;
		return ss.str();
	}

	if (!toBeCommitted.empty()) {
		ss << "Changes to be committed:" << std::endl
		   << "  (Use \"sit reset HEAD <file>...\" to unstage)" << std::endl
		   << std::endl;
		print(ss, toBeCommitted);
		ss << std::endl;
	}

	if (!notStaged.empty()) {
		ss << "Changes not staged for commit:" << std::endl
		   << "  (Use \"sit add <file>...\" to update what will be committed)" << std::endl
		   << std::endl;
		print(ss, notStaged);
		ss << std::endl;
	}

	if (!untracked.empty()) {
		ss << "Untracked files:" << std::endl
		   << "  (Use \"sit add <file>...\" to include in what will be committed)" << std::endl
		   << std::endl;
		print(ss, untracked);
		ss << std::endl;
	}

	return ss.str();
}

bool IsClean()
{
	const std::string headref(Refs::Get("HEAD"));
	if (headref == Refs::EMPTY_REF) {
		throw Util::SitException("The current repo is empty.");
	}
	const Index::CommitIndex head(headref);
	const Index::Index &index(Index::index);

	List toBeCommitted(getToBeCommitted(head, index));
	return toBeCommitted.empty();
}

}
}