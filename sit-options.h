#ifndef OPTIONS_H_
#define OPTIONS_H_
#include <boost/filesystem.hpp>
#include <iostream>
#include "sit-util.h"

#ifdef WIN32
#include <Windows.h>
#include <winnt.h>
#endif

namespace MainFrame {
void init_repo();
void add_file(const boost::filesystem::path &file);
}
#endif