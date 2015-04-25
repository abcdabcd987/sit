#include "FileSystem.hpp"

namespace Sit {
namespace FileSystem {


const boost::filesystem::path SIT_ROOT(".sit");


bool InRepo() {
    return boost::filesystem::is_directory(SIT_ROOT);
}

void AssertInRepo() {
    if (!InRepo()) {
        throw SitException("Not in a sit repo.");
    }
}


std::string Read(const boost::filesystem::path& path) {
    //HINT: use boost::iostreams::mapped_file::data
}

}
}