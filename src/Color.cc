#include "Color.hpp"

namespace Sit{
namespace Color{

const ColorType RESET       ("\033[0m",     7);
const ColorType CLS         ("\033[2J",     -1);   // Clears screen
const ColorType BLACK       ("\033[22;30m", 0);    // Black
const ColorType RED         ("\033[22;31m", 4);    // Red
const ColorType GREEN       ("\033[22;32m", 2);    // Green
const ColorType BROWN       ("\033[22;33m", 6);    // Brown / dark yellow
const ColorType BLUE        ("\033[22;34m", 1);    // Blue
const ColorType MAGENTA     ("\033[22;35m", 5);    // Magenta / purple
const ColorType CYAN        ("\033[22;36m", 3);    // Cyan
const ColorType GREY        ("\033[22;37m", 7);    // Grey / dark white
const ColorType DARKGREY    ("\033[01;30m", 8);    // Dark grey / light black
const ColorType LIGHTRED    ("\033[01;31m", 12);   // Light red
const ColorType LIGHTGREEN  ("\033[01;32m", 10);   // Light green
const ColorType YELLOW      ("\033[01;33m", 14);   // Yellow (bright)
const ColorType LIGHTBLUE   ("\033[01;34m", 9);    // Light blue
const ColorType LIGHTMAGENTA("\033[01;35m", 13);   // Light magenta / light purple
const ColorType LIGHTCYAN   ("\033[01;36m", 11);   // Light cyan
const ColorType WHITE       ("\033[01;37m", 15);   // White (bright)

}
}