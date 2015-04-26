#include "Util.hpp"

namespace Sit {
namespace Util {

std::string SHA1sum(const std::string& content)
{
	boost::uuids::detail::sha1 shaEngine;
	unsigned shaValue[5] = {0};
	shaEngine.process_bytes(content.c_str(), content.length());
	shaEngine.get_digest(shaValue);
	std::ostringstream result;
	for (auto &x : shaValue) {
		result << std::hex << x;
	}
	return result.str();
}


std::vector<std::string> SplitLine()
{
	//HINT: Use boost::algorithm::split
	return std::vector<std::string>();
}
}
}