#ifndef _SRC_UTIL_HPP
#define _SRC_UTIL_HPP
#include <string>

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
    SitException(const std::string& Message, const std::string& Detail = ""):
        message(Message), detail(Detail) {}
};



/**
 * Return the SHA1 value of `content`
 */
std::string SHA1sum(const std::string& content);



/**
 * Split the `content` into lines
 */
std::vector<std::string> SplitLine();


}
}
#endif