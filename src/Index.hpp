#ifndef _SRC_INDEX_HPP_
#define _SRC_INDEX_HPP_

#include "Objects.hpp"
#include <map>
#include <set>

namespace Sit {
namespace Index {

using IndexList = std::vector<std::pair<boost::filesystem::path, std::string>>;
using FileSet = std::map<boost::filesystem::path, std::string>;

class IndexBase {
protected:
	std::map<boost::filesystem::path, std::string> _index;
public:
	/**
	 * Check whether the `path` in the index or not
	 */
	bool InIndex(const boost::filesystem::path& path) const;

	/**
	 * Remove `path` from the index and its object file
	 */
	unsigned Remove(const boost::filesystem::path &path);

	/**
	 * Insert `file` and the sha1 of `content` to the index,
	 */
	void Insert(const boost::filesystem::path & file, const std::string & content);

	/**
	 * Return the hash value of the `path`. Throw if not exist.
	 */
	const std::string& GetID(const boost::filesystem::path &path) const;

	/**
	 * Return the _index;
	 */
	const std::map<boost::filesystem::path, std::string>& GetIndex() const;

	/*
	 * List a file which filename == path or all the files which in the path(directory)
	 */
	FileSet ListFile(const std::string &prefix) const;
};

class Index : public IndexBase {
public:
	Index(): IndexBase() {}

	/**
	 * Load the index from `.sit/index`
	 */
	void Load();

	/**
	 * Save the index to `.sit/index`
	 */
	void Save() const;

	/**
	 * Clear the index;
	 */
	void Clear();
};

class CommitIndex : public IndexBase {
	/**
	 * Load the file list from a commit
	 */
	void load(const std::string& id);

	void flattenTree(const Objects::Tree &tree, const boost::filesystem::path& prefix);
public:
	CommitIndex(const std::string& id): IndexBase() { load(id); }
};

class WorkingIndex : public IndexBase {
	/**
	 * Load the file list from the repo root.
	 */
	void load();
public:
	WorkingIndex(): IndexBase() { load(); }
};

extern Index index;

IndexBase GetIndex(const std::string &id);

}
}
#endif