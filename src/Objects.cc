#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


#include "Index.hpp"
#include "Objects.hpp"
#include "FileSystem.hpp"


namespace Sit {
namespace Objects {

boost::filesystem::path GetPath(const std::string& id)
{
	return FileSystem::REPO_ROOT / FileSystem::OBJECTS_DIR / id.substr(0, 2) / id.substr(2);
}

bool IsExist(const std::string& id)
{
	return FileSystem::IsExist(GetPath(id));
}

std::string GetBlob(const std::string& id)
{
	return FileSystem::Read(GetPath(id));
}

Tree GetTree(const std::string& id)
{
	using std::getline;
	std::istringstream ss(FileSystem::Read(GetPath(id)));
	std::string line;
	Tree tree;
	while (getline(ss, line)) {
		TreeItem item;
		item.mode = Util::FileModeToInt(line.substr(0, 6));
		item.type = line.substr(7, 4) == "tree" ? TREE : BLOB;
		item.id = line.substr(12, 40);
		item.filename = line.substr(53);

		tree.push_back(item);
	}
	return tree;
}

Commit GetCommit(const std::string& id)
{
	using std::getline;
	std::istringstream ss(FileSystem::Read(GetPath(id)));
	std::string line;
	Commit commit;

	// tree d8329fc1cc938780ffdd9f94e0d364e0ea74f579
	getline(ss, commit.tree);
	commit.tree.erase(0, 5);

	// parent 5a174a6c45eadc12180f5e9466e547777c3b3118
	getline(ss, commit.parent);
	commit.parent.erase(0, 7);

	// author Scott Chacon <schacon@gmail.com> 1243040974 -0700
	getline(ss, commit.author);
	commit.author.erase(0, 7);

	// committer Scott Chacon <schacon@gmail.com> 1243040974 -0700
	getline(ss, commit.committer);
	commit.committer.erase(0, 10);

	// Blank Line
	getline(ss, line);

	// Message
	while (getline(ss, line)) {
		commit.message += line;
		commit.message += '\n';
	}

	return commit;
}

std::string WriteBlob(const std::string& blob)
{
	const std::string sha1(Util::SHA1sum(blob));
	FileSystem::Write(GetPath(sha1), blob);
	return sha1;
}

std::string WriteTree(const Tree& tree)
{
	std::ostringstream ss;
	for (auto& item : tree) {
		ss << Util::FileModeToString(item.mode) << ' '
		   << (item.type == TREE ? "tree " : "blob ")
		   << item.id << ' '
		   << item.filename.string()
		   << '\n';
	}
	const std::string str(ss.str());
	const std::string sha1(Util::SHA1sum(str));
	FileSystem::Write(GetPath(sha1), str);
	return sha1;
}

std::string WriteCommit(const Commit& commit)
{
	std::ostringstream ss;
	ss << "tree " << commit.tree << '\n'
	   << "parent " << commit.parent << '\n'
	   << "author " << commit.author << '\n'
	   << "committer " << commit.committer << '\n'
	   << '\n'
	   << commit.message;
	const std::string str(ss.str());
	const std::string sha1(Util::SHA1sum(str));
	FileSystem::Write(GetPath(sha1), str);
	return sha1;
}

struct IndexTreeItem;
typedef std::map<std::string, IndexTreeItem> IndexTree;
struct IndexTreeItem
{
	std::string filename;
	std::string blobid;
	IndexTree *tree;
};

std::string writeIndexTree(const IndexTree& idt)
{
	Tree tree;
	for (const auto &i : idt) {
		TreeItem item;
		if (i.second.tree) {
			item.mode = 040000;
			item.type = Objects::TREE;
			item.id = writeIndexTree(*i.second.tree);
			item.filename = i.second.filename;
		} else {
			item.mode = 0100644;
			item.type = Objects::BLOB;
			item.id = i.second.blobid;
			item.filename = i.second.filename;
		}
		tree.push_back(item);
	}
	return WriteTree(tree);
}

IndexTree* makeIndexTree(const Index::Index& indexobj)
{
	const auto &index = indexobj.GetIndex();
	IndexTree *tree = new IndexTree();
	for (const auto &i : index) {
		std::vector<std::string> dirs;
		boost::split(dirs, i.first.generic_string(), boost::is_any_of("/"));
		dirs.erase(dirs.end() - 1);
		IndexTree *parent = tree;
		for (const auto &dir : dirs) {
			auto iter = parent->find(dir);
			if (iter != parent->end()) {
				parent = iter->second.tree;
			} else {
				IndexTree *t = new IndexTree;
				parent->insert(std::make_pair(dir, IndexTreeItem({dir, "", t})));
				parent = t;
			}
		}
		const std::string filename(i.first.filename().string());
		const IndexTreeItem item({filename, i.second, 0});
		parent->insert(std::make_pair(filename, item));
	}
	return tree;
}

void deleteIndexTree(IndexTree *tree)
{
	for (const auto &i : *tree) {
		if (i.second.tree) {
			deleteIndexTree(i.second.tree);
		}
	}
	delete tree;
}

std::string WriteIndex()
{
	IndexTree *tree = makeIndexTree(Index::index);
	std::string id(writeIndexTree(*tree));
	deleteIndexTree(tree);
	return id;
}

}
}