#ifndef _SRC_DIFF_HPP_
#define _SRC_DIFF_HPP_
#include <boost/filesystem.hpp>

#include <string>
#include <map>
#include "Index.hpp"

namespace Sit {
namespace Diff {

/**
 * File Status
 */
enum FileStatus { Added, Modified, Deleted, Same, Untracked };

/**
 * An item of a `Diff` object
 */
struct DiffItem {
	std::string baseid;           // Source object ID
	std::string targetid;         // Target object ID
	boost::filesystem::path path; // Item path
	FileStatus status;            // File status
};

/**
 * A `DiffList` object is a map of the file status of each file
 */
typedef std::map<boost::filesystem::path, DiffItem> DiffList;

/**
 * Compare `base` to `target` in brief
 */
DiffList Diff(const Index::IndexBase& base, const Index::IndexBase& target);

/**
 * Return the diff string between two index
 */
std::string DiffIndex(const std::string &baseID, const std::string &targetID);

}
}
#endif