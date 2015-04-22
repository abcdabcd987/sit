#include "sit-sha1.h"

namespace Util {
SitSha::SitSha()
{
	this->shaEngine.reset();
	this->processed = false;
}

SitSha::SitSha(const boost::filesystem::path &_inputFile)
	: inputFile(_inputFile)
{
	this->shaEngine.reset();
	this->processed = false;
}

void SitSha::Reset()
{
	this->shaEngine.reset();
	this->processed = false;
}

void SitSha::SetFile(const boost::filesystem::path &_inputFile)
{
	this->inputFile = _inputFile;
	if (this->processed) {
		this->processed = false;
		this->shaEngine.reset();
	}
}

void SitSha::Process()
{
	using namespace boost::filesystem;
	try {
		auto ifsize = boost::filesystem::file_size(inputFile);
		char *s = new char [ifsize];
		ifstream ifile(inputFile, std::ios::in | std::ios::binary);

		ifile.read(s, ifsize);
		this->shaEngine.process_bytes(s, ifsize);
		this->shaEngine.get_digest(this->shaValue);
		this->processed = true;

		delete[] s;
		ifile.close();
	} catch (const filesystem_error &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

std::string SitSha::ResultStr()
{
	if (!this->processed) {
		this->Process();
	}
	std::ostringstream oss;
	for (auto x : shaValue) {
		oss << std::hex << x;
	}
	return oss.str();
}

std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned> SitSha::ResultTup()
{
	if (!this->processed) {
		this->Process();
	}
	return std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned>(
		this->shaValue[0],
		this->shaValue[1],
		this->shaValue[2],
		this->shaValue[3],
		this->shaValue[4]
		);
}
}