#ifndef _SRC_CONFIG_HPP_
#define _SRC_CONFIG_HPP_
#include <map>
#include <string>

namespace Sit {
namespace Config {

/**
 * Constant: Config key not found
 */
extern const std::string NOT_FOUND;

/**
 * Config type
 */
typedef std::map<std::string, std::string> Config;

/**
 * Get configuration
 */
std::string Get(const std::string& key);

/**
 * Set configuration
 */
void Set(const std::string& key, const std::string& value);

}
}

#endif