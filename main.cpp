#define _GLIBCXX_USE_CXX11_ABI 0
#define DEBUG
#include <iostream>
#include "spm.h"
#include <unistd.h> // getuid
using namespace std;
int main(int argc, char** argv) {
	std::cout << "Using spm v1.0 and cpphttplib v0.14.1" << std::endl;
	if (getuid()) {
		std::cout << "Please run as root!";
		return 1;
	}
	if (argc < 2) {
		std::cout << "Usage: spm <command> <args>\n";
		return 1;
	}
	const char* ch = argv[1];
	string s = ch;
	string addRepo = "add-repository";
	string removeRepo = "remove-repository";
	string update = "update";
	string install = "install";
	string uninstall = "remove";
	if (s.compare(uninstall) == 0) {
		if (argc < 3) {
			std::cout << "Usage: spm remove <pakage name>\n";
			return 1;
		}
		return spm::remove(argv[2]);

	}
	if (s.compare(install) == 0) {
		if (argc < 3) {
			std::cout << "Usage: spm install <pakage name>\n";
			return 1;
		}
		return spm::install(argv[2]);

	}
	if (s.compare(addRepo) == 0) {
		if (argc < 3) {
			std::cout << "Usage: spm add-repository <url>\n";
			return 1;
		}
		return spm::add_repository(argv[2]);
		
	}
	if (s.compare(removeRepo) == 0) {
		if (argc < 3) {
			std::cout << "Usage: spm remove-repository <url>\n";
			return 1;
		}
		return spm::remove_repository(argv[2]);
		
	}
	if (s.compare(update) == 0) {
		return spm::update();
		
	}
	
}
