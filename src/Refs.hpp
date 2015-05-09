#ifndef _SRC_REFS_HPP
#define _SRC_REFS_HPP

#include <string>

namespace Sit {
namespace Refs {

extern const std::string EMPTY_REF;

/**
 * `name` => `refs/heads/name`
 */
std::string Local(const std::string& name);

/**
 * Get the sha1 value of ref path.
 * Throw if not exist.
 */
std::string Get(const std::string& path);

/**
 * Set the sha1 value of ref path.
 */
void Set(const std::string& ref, const std::string& id);

}
}

#endif