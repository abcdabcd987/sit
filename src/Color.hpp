#ifndef _SRC_COLOR_HPP
#define _SRC_COLOR_HPP
#include <string>

#ifdef _WIN32
#include <windows.h>  // for WinAPI and Sleep()
#define _NO_OLDNAMES  // for MinGW compatibility
#undef min
#endif

namespace Sit {
namespace Color {

class ColorType {
#ifdef _WIN32
	int windows;
#else
	std::string ansi;
#endif
public:
	friend std::ostream& operator<<(std::ostream &out, const ColorType& c);
#ifdef _WIN32
	ColorType(const std::string &a, const int w): windows(w) {}
#else
	ColorType(const std::string &a, const int w): ansi(a) {}
#endif
};

inline std::ostream& operator<<(std::ostream &out, const ColorType& c) {
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)c.windows);
#else
	out << c.ansi;
#endif
	return out;
}

extern const ColorType RESET       ;
extern const ColorType CLS         ; // Clears screen
extern const ColorType BLACK       ; // Black
extern const ColorType RED         ; // Red
extern const ColorType GREEN       ; // Green
extern const ColorType BROWN       ; // Brown / dark yellow
extern const ColorType BLUE        ; // Blue
extern const ColorType MAGENTA     ; // Magenta / purple
extern const ColorType CYAN        ; // Cyan
extern const ColorType GREY        ; // Grey / dark white
extern const ColorType DARKGREY    ; // Dark grey / light black
extern const ColorType LIGHTRED    ; // Light red
extern const ColorType LIGHTGREEN  ; // Light green
extern const ColorType YELLOW      ; // Yellow (bright)
extern const ColorType LIGHTBLUE   ; // Light blue
extern const ColorType LIGHTMAGENTA; // Light magenta / light purple
extern const ColorType LIGHTCYAN   ; // Light cyan
extern const ColorType WHITE       ; // White (bright)

}
}

#endif
