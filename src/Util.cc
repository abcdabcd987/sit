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
	//TODO
	//HINT: Use boost::algorithm::split
	return std::vector<std::string>();
}

std::string FirstLine(const std::string& str)
{
	std::string res;
	for (auto ch : str) {
		if (ch == '\n') {
			break;
		} else {
			res += ch;
		}
	}
	return res;
}

int FileModeToInt(const std::string& mode)
{
	int res = 0;
	for (int i = 0; i < 6; ++i)
		res = res * 8 + mode[i] - '0';
	return res;
}

std::string FileModeToString(int mode)
{
	std::string res("000000");
	for (int i = 0; i < 6; ++i) {
		res[5-i] = '0' + mode % 8;
		mode /= 8;
	}
	return res;
}

std::string AuthorString(const std::string& name, const std::string& email, const std::string& datetime)
{
	return name + " <" + email + "> " + datetime;
}

}
}