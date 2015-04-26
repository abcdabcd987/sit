#include "Core.hpp"

int main(int argc, char** argv)
{
	if (argv[1] == std::string("init")) {
		Sit::Core::Init();
	}
	if (argv[1] == std::string("add")) {
		Sit::Core::Add(argv[2]);
	}
	if (argv[1] == std::string("remove")) {
		Sit::Core::Rm(argv[2]);
	}
	return 0;
}