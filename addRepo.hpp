#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "include/httplib.h"
#include <unistd.h> // getuid
#include "MirrorList.hpp"
class RepoAdder {
public:
	int addRepo(const char* url) {
		httplib::Client cli(url);
		std::cout << "Requesting " << url << "/index" << "\n";
		httplib::Result res;
		try
		{
           res = cli.Get("/index");
		}
		catch (const std::exception&)
		{
			std::cout << "Coud not connect to server!\n" << "Repository not added!\n";
		}
		
		if (res->status == 200) {
			std::cout << "Ok\n";
			return saveToMirrorList(url);
		}
		std::cout << "Server returned:" << res->status <<"\n" << "Repository not added!";
		return 1;


	}
private:
	int saveToMirrorList(std::string mirror) {
		std::ifstream in("/etc/evo/mirror.list");
		const char* path = "/etc/evo/mirror.list";
		std::ofstream file(path); //open in constructor
		if (access(path, F_OK) != -1) {
			
		}
		else {
			std::cout << "Can't read mirror list\n";
			return 1;
		}
		std::ofstream outfile;

		outfile.open(path, std::ios_base::app); // append instead of overwrite
		outfile << "\nserver=" + mirror;
		MirrorList list;
		list.load();
		return 0;
	}
};