#ifndef SIT_SHA_H_
#define SIT_SHA_H_

#include <boost/uuid/sha1.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <string>
#include <array>

namespace util {

class sit_sha {
	boost::filesystem::path input_file;
	boost::uuids::detail::sha1 sha_engine;
	unsigned sha_value[5];
	bool processed;
public:
	sit_sha();
	sit_sha(const boost::filesystem::path &_input_file);
	void reset();
	void set_file(const boost::filesystem::path &_input_file);
	/**
	 * Get bytes from @input_file and run sha1 algorithm, final result is saved in @sha_value.
	 */
	void process();
	/**
	 * Convert the result of sha1 algorithm into std::string
	 * If the result has not been calculate (@processed == false), function process() would call before converting.
	 * @return the sha1 result represented by std::string
	 */
	std::string result_str();
	/**
	 * Convert the result of sha1 algorithm into std::tuple
	 * If the result has not been calculate (@processed == false), function process() would call before converting.
	 * @return the sha1 result represented by five integers.
	 */
	std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned> result_tup();
};

}
#endif