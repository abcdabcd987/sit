#include "Util.hpp"
#include <cctype>

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
		result << std::setw(8) << std::setfill('0') << std::hex << x;
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
	for (int i = 0; i < 6; ++i) {
		if (!isdigit(mode[i])) {
			throw SitException("Fatal: Bad File Mode.");
		}
		res = res * 8 + mode[i] - '0';
	}
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

std::string SHA1Complete(std::string _id)
{
	if (_id == "" || _id == "index" || _id == "master" || _id == "HEAD" || _id == "work")
		return _id;
	boost::filesystem::path path = _id.substr(0, 2);
	boost::filesystem::path filename = _id.substr(2);
	boost::filesystem::path result;
	auto fileList = FileSystem::ListRecursive(FileSystem::REPO_ROOT / FileSystem::OBJECTS_DIR / path, false);
	for (const auto &file : fileList) {
		if (!FileSystem::IsDirectory(file)) {
			if (file.filename().generic_string().find(filename.generic_string()) != std::string::npos) {
				if (result.empty()) {
					result = file.filename();
				} else {
					throw SitException("Fatal: There are several matches.");
				}
			}
		}
	}
	if (result.empty()) {
		throw SitException("Fatal: No such a object.");
	} else {
		return path.generic_string() + result.generic_string();
	}
}

}
}