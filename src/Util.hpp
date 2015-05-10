#ifndef _SRC_UTIL_HPP
#define _SRC_UTIL_HPP
#include <sstream>
#include <iomanip>
#include "FileSystem.hpp"

#include <string>
#include <vector>
#include <boost/uuid/sha1.hpp>

namespace Sit {
namespace Util {

/**
 * The struct to represent exception
 */
struct SitException {
	/**
	 * The message intended to be shown to user
	 */
	const std::string message;


	/**
	 * The details for debug
	 */
	const std::string detail;
	SitException(const std::string& Message, const std::string& Detail = "")
		: message(Message), detail(Detail) {}
};

/**
 * Return the SHA1 value of `content`
 */
std::string SHA1sum(const std::string &content);

/**
 * Split the `content` into lines
 */
std::vector<std::string> SplitLine();

/**
 * Return the first line of the string
 */
std::string FirstLine(const std::string& str);

/**
 * Convert file mode string to int
 */
int FileModeToInt(const std::string& mode);

/**
 * Convert file mode to string
 */
std::string FileModeToString(const int mode);

/**
 * Generate author string for committing
 */
std::string AuthorString(const std::string& name, const std::string& email, const std::string& datetime);

/*
 *Complete SHA1 value with exists objects.
 */
std::string SHA1Complete(std::string _id);
}
}
#endif