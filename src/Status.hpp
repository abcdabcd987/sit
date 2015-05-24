#ifndef _SRC_STATUS_HPP_
#define _SRC_STATUS_HPP_

#include "Index.hpp"
#include "Diff.hpp"
#include <ostream>

namespace Sit {
namespace Status {

void PrintStatus(std::ostream& out);

/**
 * Whether the repo is clean
 */
bool IsClean();

}
}
#endif