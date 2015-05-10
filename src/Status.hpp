#ifndef _SRC_STATUS_HPP_
#define _SRC_STATUS_HPP_

#include "Index.hpp"
#include "Diff.hpp"

namespace Sit {
namespace Status {

std::string StatusString();

/**
 * Whether the repo is clean
 */
bool IsClean();

}
}
#endif