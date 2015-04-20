#ifndef SIT_SHA_H_
#define SIT_SHA_H_

#include <boost/uuid/sha1.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sstream>
#include <iostream>

#include <string>
#include <array>

namespace util {

class sit_sha {
	boost::filesystem::path input_file;
	boost::uuids::detail::sha1 sha_engine;
	unsigned int sha_value[5];
	bool processed;
public:
	sit_sha();
	sit_sha(const boost::filesystem::path &_input_file);
	void reset();
	void set_file(const boost::filesystem::path &_input_file);
	void process();
	std::string result_str();
	std::tuple<int, int, int, int, int> result_tup();
};

}
#endif