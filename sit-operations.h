#ifndef OPTIONS_H_
#define OPTIONS_H_
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <iostream>
#include "sit-util.h"

#ifdef WIN32
#include <Windows.h>
#include <winnt.h>
#endif

namespace MainFrame {
void InitRepo();
void AddFile(const boost::filesystem::path &file);
}
#endif