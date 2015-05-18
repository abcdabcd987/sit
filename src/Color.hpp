#ifndef _SRC_COLOR_HPP
#define _SRC_COLOR_HPP
#include <string>

namespace Sit {
namespace Color {

const std::string RESET = "\033[0m";
const std::string CLS = "\033[2J";              // Clears screen
const std::string BLACK = "\033[22;30m";        // Black
const std::string RED = "\033[22;31m";          // Red
const std::string GREEN = "\033[22;32m";        // Green
const std::string BROWN = "\033[22;33m";        // Brown / dark yellow
const std::string BLUE = "\033[22;34m";         // Blue
const std::string MAGENTA = "\033[22;35m";      // Magenta / purple
const std::string CYAN = "\033[22;36m";         // Cyan
const std::string GREY = "\033[22;37m";         // Grey / dark white
const std::string DARKGREY = "\033[01;30m";     // Dark grey / light black
const std::string LIGHTRED = "\033[01;31m";     // Light red
const std::string LIGHTGREEN = "\033[01;32m";   // Light green
const std::string YELLOW = "\033[01;33m";       // Yellow (bright)
const std::string LIGHTBLUE = "\033[01;34m";    // Light blue
const std::string LIGHTMAGENTA = "\033[01;35m"; // Light magenta / light purple
const std::string LIGHTCYAN = "\033[01;36m";    // Light cyan
const std::string WHITE = "\033[01;37m";        // White (bright)

}
}

#endif
