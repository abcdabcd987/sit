#ifndef _SRC_DIFF_HPP_
#define _SRC_DIFF_HPP_
#include <boost/filesystem.hpp>

#include <string>
#include <map>

namespace Sit {
namespace Diff {


/**
 * File Status
 */
enum FileStatus { Added, Modified, Deleted, Same };


/**
 * An item of a `Diff` object
 */
struct DiffItem {
	FileStatus status;            // File status
	std::string diff;             // Diff string
};


/**
 * A `Diff` object is a map of the file status of each file
 */
typedef std::map<boost::filesystem::path, DiffItem> Diff;


/**
 * Compare `base` to `target` in detail, that is, offer the diff string
 */
Diff DetailDiff(const std::string& base, const std::string& target);


/**
 * Compare `base` to `target` according to SHA1 without diff string
 */
Diff FastDiff(const std::string& base, const std::string& target);




}
}
#endif