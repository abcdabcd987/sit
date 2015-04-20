#include "sit-sha1.h"

namespace util {
sit_sha::sit_sha()
{
	this->sha_engine.reset();
	this->processed = false;
}

sit_sha::sit_sha(const boost::filesystem::path &_input_file)
	: input_file(_input_file)
{
	this->sha_engine.reset();
	this->processed = false;
}

void sit_sha::reset()
{
	this->sha_engine.reset();
	this->processed = false;
}

void sit_sha::set_file(const boost::filesystem::path &_input_file)
{
	this->input_file = _input_file;
	if (this->processed) {
		this->processed = false;
		this->sha_engine.reset();
	}
}

void sit_sha::process()
{
	using namespace boost::filesystem;
	try {
		auto ifsize = boost::filesystem::file_size(input_file);
		char *s = new char [ifsize];
		ifstream ifile(input_file, std::ios::in | std::ios::binary);

		ifile.read(s, ifsize);
		this->sha_engine.process_bytes(s, ifsize);
		this->sha_engine.get_digest(this->sha_value);
		this->processed = true;

		delete[] s;
		ifile.close();
	} catch (const filesystem_error &ec) {
		std::cerr << ec.what() << std::endl;
	}
}

std::string sit_sha::result_str()
{
	if (!this->processed) {
		this->process();
	}
	std::ostringstream oss;
	for (auto x : sha_value) {
		oss << std::ios::hex << x;
	}
	return oss.str();
}

std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned> sit_sha::result_tup()
{
	if (!this->processed) {
		this->process();
	}
	return std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned>(
		this->sha_value[0],
		this->sha_value[1],
		this->sha_value[2],
		this->sha_value[3],
		this->sha_value[4]
		);
}

}