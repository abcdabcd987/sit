#ifndef SIT_OPERATIONS_H_
#define SIT_OPERATIONS_H_
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
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