#include <boost/filesystem.hpp>
#include <vector>
#include <sstream>

#include "Diff.hpp"
#include "Index.hpp"
#include "Status.hpp"
#include "FileSystem.hpp"
#include "Refs.hpp"
#include "Util.hpp"

#include <iostream>

namespace Sit {
namespace Status {

struct StatusItem {
	boost::filesystem::path path;
	Diff::FileStatus status;
};
inline bool operator<(const StatusItem& lhs, const StatusItem& rhs)
{
	return lhs.path < rhs.path;
}
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
	std::sort(v.begin(), v.end());
	return v;
}

List getToBeCommitted(const Index::IndexBase &head, const Index::Index &index)
{
	List v;
	const Diff::DiffList diff(Diff::Diff(head, index));
	for (const auto &item : diff) {
		if (item.second.status != Diff::Same) {
			v.push_back(StatusItem({item.first, item.second.status}));
		}
	}
	std::sort(v.begin(), v.end());
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
	std::sort(v.begin(), v.end());
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

void PrintStatus(std::ostream &out)
{
	const std::string headref(Refs::Get("HEAD"));

	Index::IndexBase head;
	if (headref != Objects::EMPTY_OBJECT) {
		head = Index::CommitIndex(headref);
	}
	const Index::WorkingIndex work;
	const Index::Index &index(Index::index);

	List untracked(getUntracked(work, index));
	List toBeCommitted(getToBeCommitted(head, index));
	List notStaged(getNotStaged(index, work));

	out << "On branch: master" << std::endl;
	if (headref != Refs::Get("master")) {
		out << "HEAD detached at " << headref << std::endl;
	}

	if (untracked.empty() && toBeCommitted.empty() && notStaged.empty()) {
		out << "Nothing to commit, working directory clean" << std::endl;
		return;
	}

	if (!toBeCommitted.empty()) {
		out << "Changes to be committed:" << std::endl
		   << "  (Use \"sit reset -- <file>\" to unstage)" << std::endl
		   << std::endl;
		out << Color::GREEN;
		print(out, toBeCommitted);
		out << Color::RESET;
		out << std::endl;
	}

	if (!notStaged.empty()) {
		out << "Changes not staged for commit:" << std::endl
		   << "  (Use \"sit add <file>...\" to update what will be committed)" << std::endl
		   << std::endl;
		out << Color::RED;
		print(out, notStaged);
		out << Color::RESET;
		out << std::endl;
	}

	if (!untracked.empty()) {
		out << "Untracked files:" << std::endl
		   << "  (Use \"sit add <file>...\" to include in what will be committed)" << std::endl
		   << std::endl;
		out << Color::BROWN;
		print(out, untracked);
		out << Color::RESET;
		out << std::endl;
	}
}

bool IsClean()
{
	const std::string headref(Refs::Get("HEAD"));
	if (headref == Objects::EMPTY_OBJECT) {
		throw Util::SitException("The current repo is empty.");
	}
	const Index::CommitIndex head(headref);
	const Index::Index &index(Index::index);

	List toBeCommitted(getToBeCommitted(head, index));
	return toBeCommitted.empty();
}

}
}