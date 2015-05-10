#ifndef _SRC_OBJECTS_HPP
#define _SRC_OBJECTS_HPP

#include <boost/filesystem.hpp>
#include <string>
#include <vector>

namespace Sit {
namespace Objects {

/**
 * Object type
 */
enum ObjectType { BLOB, TREE, COMMIT };

/**
 * A tree item
 */
struct TreeItem {
	int mode;                         // Linux permission mode
	ObjectType type;                  // The type of the item, either blob or tree
	std::string id;                   // The sha1 of the item
	boost::filesystem::path filename; // The filename
};

/**
 * The `Tree` is actually a list of `TreeItem`
 * Like this:
 *     100644 blob a906cb2a4a904a152e80877d4088654daad0c859    README
 *     100644 blob 8f94139338f9404f26296befa88755fc2598c289    Rakefile
 *     040000 tree 99f1a6d12cb4b6f19c8655fca46c3ecf317074e0    lib
 */
typedef std::vector<TreeItem> Tree;

/**
 * The item saving a commit
 * Like this:
 *     tree d8329fc1cc938780ffdd9f94e0d364e0ea74f579
 *     parent 5a174a6c45eadc12180f5e9466e547777c3b3118
 *     author Scott Chacon <schacon@gmail.com> 1243040974 -0700
 *     committer Scott Chacon <schacon@gmail.com> 1243040974 -0700
 *
 *     This is my second commit.
 */
struct Commit {
	std::string tree;      // The sha1 value of the repo root tree
	std::string parent;    // The sha1 value of the parent commit.
	std::string author;    // The author's name, email, timestamp, timezone
	std::string committer; // The committer's name, email, timestamp, timezone
	std::string message;   // Commit log
};

/**
 * Get object filesystem path
 */
boost::filesystem::path GetPath(const std::string& id);

/**
 * Check whether the object which has sha1 value `id` exists or not
 */
bool IsExist(const std::string& id);

/**
 * Get the blob which has sha1 value `id`.
 * Throw exception if not found or not a blob
 */
std::string GetBlob(const std::string& id);

/**
 * Get the tree which has sh1 value `id`.
 * Throw exception if not found or not a tree
 */
Tree GetTree(const std::string& id);

/**
 * Get the commit which has sh1 value `id`.
 * Throw exception if not found or not a commit
 */
Commit GetCommit(const std::string& id);

///**
// * Generate the additional header of the content
// * Additional header: OBJECT_TYPE CONTENT_LENGTH \n
// * Ref: http://git-scm.com/book/zh/v1/Git-%E5%86%85%E9%83%A8%E5%8E%9F%E7%90%86-Git-%E5%AF%B9%E8%B1%A1
// */
//void GenHeader(const std::string& content);
// Seems not useful??
// I asked a question:
// http://stackoverflow.com/questions/29914417/why-git-prepends-a-type-length-0-header-in-object-files/29914642

/**
 * Write the blob object `blob` to file with additional header.
 * Return object SHA1.
 */
std::string WriteBlob(const std::string& blob);

/**
 * Write the tree object `tree` to file with additional header.
 * Return object SHA1.
 */
std::string WriteTree(const Tree& tree);

/**
 * Write the commit object `commit` to file with additional header.
 * Return object SHA1.
 */
std::string WriteCommit(const Commit& commit);

/**
 * Write the tree object constructed from index
 * Return tree object SHA1.
 */
std::string WriteIndex();


}
}
#endif
