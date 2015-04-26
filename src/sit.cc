#include "Core.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	if (argv[1] == std::string("init")) {
		Sit::Core::Init();
	}
	if (argv[1] == std::string("add")) {
		std::cout << argv[2] << std::endl;
		Sit::Core::Add(argv[2]);
	}
	if (argv[1] == std::string("remove")) {
		std::cout << argv[2] << std::endl;
		Sit::Core::Rm(argv[2]);
	}
	return 0;
}