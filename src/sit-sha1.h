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

namespace Util {

class SitSha {
	boost::filesystem::path inputFile;
	boost::uuids::detail::sha1 shaEngine;
	unsigned shaValue[5];
	bool processed;
public:
	SitSha();
	SitSha(const boost::filesystem::path &_inputFile);
	void Reset();
	void SetFile(const boost::filesystem::path &_inputFile);
	/**
	 * Get bytes from @input_file and run sha1 algorithm, final result is saved in @sha_value.
	 */
	void Process();
	/**
	 * Convert the result of sha1 algorithm into std::string
	 * If the result has not been calculate (@processed == false), function process() would call before converting.
	 * @return the sha1 result represented by std::string
	 */
	std::string ResultStr();
	/**
	 * Convert the result of sha1 algorithm into std::tuple
	 * If the result has not been calculate (@processed == false), function process() would call before converting.
	 * @return the sha1 result represented by five integers.
	 */
	std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned> ResultTup();
};

}
#endif