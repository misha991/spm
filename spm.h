#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>  
#include <fstream>
#include <stdio.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include <string>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdexcept>
#include <string>
#include <math.h>
#include "httplib.h"
#include "indicators.hpp"
namespace spm {
	
	namespace core {
		class Downloader {
		private:
			static int download_progress_callback(void* clientp, curl_off_t dltotal,
				curl_off_t dlnow, curl_off_t ultotal,
				curl_off_t ulnow)
			{
				indicators::ProgressBar* progress_bar =
					static_cast<indicators::ProgressBar*>(clientp);

				if (progress_bar->is_completed())
				{
					;
				}
				else if (dltotal == 0)
				{
					progress_bar->set_progress(0);
				}
				else
				{
					int percentage =
						static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100;
					progress_bar->set_progress(percentage);
				}

				// If your callback function returns CURL_PROGRESSFUNC_CONTINUE it will
				// cause libcurl to continue executing the default progress function. return
				// CURL_PROGRESSFUNC_CONTINUE;

				return 0;
			}

			static int download_progress_default_callback(void* clientp, curl_off_t dltotal,
				curl_off_t dlnow, curl_off_t ultotal,
				curl_off_t ulnow)
			{
				return CURL_PROGRESSFUNC_CONTINUE;
			}

			std::string extract_file_name(const std::string& url)
			{
				int i = url.size();
				for (; i >= 0; i--)
				{
					if (url[i] == '/')
					{
						break;
					}
				}

				return url.substr(i + 1, url.size() - 1);
			}
		public:
			bool download(const std::string& url, const std::string& file_path)
			{
				const std::string file_name = extract_file_name(url);

				// Hide cursor
				indicators::show_console_cursor(false);

				indicators::ProgressBar progress_bar{
					indicators::option::BarWidth{30}, indicators::option::Start{" ["},
					indicators::option::Fill{"#"}, indicators::option::Lead{"#"},
					indicators::option::Remainder{"-"}, indicators::option::End{"]"},
					indicators::option::PrefixText{file_name},
					// indicators::option::ForegroundColor{indicators::Color::yellow},
					indicators::option::ShowElapsedTime{true},
					indicators::option::ShowRemainingTime{true},
					// indicators::option::FontStyles{
					//     std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
				};

				CURL* curl;
				FILE* fp;
				CURLcode res;
				curl = curl_easy_init();
				if (curl)
				{
					fp = fopen(file_path.c_str(), "wb");
					curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
					curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION,
						download_progress_callback);
					curl_easy_setopt(curl, CURLOPT_XFERINFODATA,
						static_cast<void*>(&progress_bar));
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
					// Perform a file transfer synchronously.
					res = curl_easy_perform(curl);
					curl_easy_cleanup(curl);
					fclose(fp);
				}
				indicators::show_console_cursor(true);
				if (res == CURLE_OK)
				{
					std::cout << "Downloaded file " << file_name << std::endl;
					return true;
				}
				else
				{
					std::cout << "Error while downloading: " << res << std::endl;
					return false;
				}
			}

		};
		std::vector<std::string> split(std::string str, char delim)
		{
			std::vector<std::string> result;
			auto left = str.begin();
			for (auto it = left; it != str.end(); ++it)
			{
				if (*it == delim)
				{
					result.emplace_back(&*left, it - left);
					left = it + 1;
				}
			}
			if (left != str.end())
				result.emplace_back(&*left, str.end() - left);
			return result;
		}
		class ProgressBar {
			private:
				
				float progress = 0.0;
				
				float max = 1.0;
				
			public:
				float step;
				int color;
				std::string text;
			ProgressBar(std::string text,int color,float st) {
				this->color = color;
				this->text = text;
				this->step = max / st;
			}
			void tick() {
				indicators::ProgressBar progress_bar{
					indicators::option::BarWidth{30}, indicators::option::Start{" ["},
					indicators::option::Fill{"#"}, indicators::option::Lead{"#"},
					indicators::option::Remainder{"-"}, indicators::option::End{"]"},
					indicators::option::PrefixText{text},
					// indicators::option::ForegroundColor{indicators::Color::yellow},
					indicators::option::ShowElapsedTime{true},
					indicators::option::ShowRemainingTime{true},
					// indicators::option::FontStyles{
					//     std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}
				};
				if(progress < max) {
					progress = progress + step; 
					progress_bar.set_progress(progress * 100);
					

					
				}
				//std::cout << std::endl;
			}
			void reset() {
				std::cout << std::endl;
				progress = 0.0;
			}
		};
		class File {
		private:

		public:
			const char* path;
			File() {

			}
			File(const char* path) {
				this->path = path;
				if (access(path, F_OK) != -1) {
					//#ifdef DEBUG

					//#endif // DEBUG
				}
				else {
					std::ofstream outfile(path);

					outfile << "" << std::endl;

					outfile.close();
					if (access(path, F_OK) != -1) {

					}
					else {
						std::cout << "Error reading file" << std::endl;
						exit(1);
					}

				}
			}
			std::string readAllText() {
				std::ifstream t(path);
				std::string str((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
				return str;
			}
			int copyTo(std::string copyPath) {
				std::string line;
				// For writing text file 
				// Creating ofstream & ifstream class object 
				std::ifstream ini_file = std::ifstream(path);
				 // This is the original file 
				std::ofstream out_file = std::ofstream(copyPath);
				if (ini_file && out_file) {

					while (getline(ini_file, line)) {
						out_file << line << "\n";
					}
				}
				else {
					return 1;
				}
				// Closing file 
				ini_file.close();
				out_file.close();
				return 0;
			}
			int append(std::string text) {
				std::ofstream out;

				// std::ios::app is the open mode "append" meaning
				// new data will be written to the end of the file.
				out.open(path, std::ios::app);

				std::string str = text;
				out << str;
				out.close();
				return 0;
			}
			int appendLine(std::string text) {
				std::ofstream out;

				// std::ios::app is the open mode "append" meaning
				// new data will be written to the end of the file.
				out.open(path, std::ios::app);

				std::string str = text;
				out << str << std::endl;
				out.close();
				return 0;
			}
			int removeLine(std::string rLine) {
				std::string line;
				std::ifstream fin;
				//mirror list path
				fin.open(path);
				// contents of path must be copied to a temp file then
				// renamed back to the path file
				mkdir("/tmp/spm", 0777);
				std::ofstream temp;
				temp.open("/tmp/spm/spm_rline_temp", std::ios::app);
				while (getline(fin, line)) {
					// write all lines to temp other than the line marked for erasing
					if (line != rLine)
						temp << line << std::endl;
				}
				//create temp dir
				
				temp.close();
				fin.close();
				//remove previos version of mirror list;
				std::remove(path);
				std::ifstream in("/tmp/spm/spm_rline_temp", std::ios::in | std::ios::binary);
				std::ofstream out(path, std::ios::out | std::ios::binary);
				out << in.rdbuf();
				//removing temp file
				std::remove("/tmp/spm/spm_rline_temp");
				in.close();
				out.close();
				return 0;
			}
		};
	}
#pragma region models
	class Mirror {
	public:
		std::string url;
		Mirror() {
		
		}
		
		Mirror(const char* u) {
			url = u;
		}
		int getCount(const char* path){
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("server") == 0) {
								count= count + 1;
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif //DEBUG
							}
						}
					}
				}
			}
			in.close();
			return count;
		}
		Mirror getFirstByKey(const char* path) {
			if (access(path, F_OK) != -1) {
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif //DEBUG
				return Mirror("ERR_READ_FILE");
			}
			std::ifstream in(path);
			std::string line;
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "" << value << "}" << std::endl;
#endif // DEBUG
							if (key.compare("server") == 0) {
								const char* _char = value.c_str();
								return Mirror(_char);
							}
						}
					}
				}
			}
		}
		Mirror* getAllByKey(const char* path) {
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {

#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("server") == 0) {
								count = count + 1;
							}
						}
					}
				}
			}
			in.close();
			in.open(path);
			line = "";
			int i = 0;
			Mirror* values = new Mirror[count];
			//one more read
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "=" << value << "}" << std::endl;
#endif // DEBUG
							//check if equels key
							if (key.compare("server") == 0) {
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif // DEBUG
								values[i] =Mirror(value.c_str());
								i++;
							}
						}
					}
				}
			}
			in.close();
			return values;
		}
	};
	class Directory {
	public:
		std::string url;
		Directory() {

		}

		Directory(const char* u) {
			url = u;
		}
		int getCount(const char* path) {
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("dir") == 0) {
								count = count + 1;
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif //DEBUG
							}
						}
					}
				}
			}
			in.close();
			return count;
		}
		Directory getFirstByKey(const char* path) {
			if (access(path, F_OK) != -1) {
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif //DEBUG
				return Directory("ERR_READ_FILE");
			}
			std::ifstream in(path);
			std::string line;
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "" << value << "}" << std::endl;
#endif // DEBUG
							if (key.compare("dir") == 0) {
								const char* _char = value.c_str();
								return Directory(_char);
							}
						}
					}
				}
			}
		}
		Directory* getAllByKey(const char* path) {
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {

#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("dir") == 0) {
								count = count + 1;
							}
						}
					}
				}
			}
			in.close();
			in.open(path);
			line = "";
			int i = 0;
			Directory* values = new Directory[count];
			//one more read
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "=" << value << "}" << std::endl;
#endif // DEBUG
							//check if equels key
							if (key.compare("dir") == 0) {
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif // DEBUG
								values[i] = Directory(value.c_str());
								i++;
							}
						}
					}
				}
			}
			in.close();
			return values;
		}
	};
	class Pakage {
	public:
		std::string url;
		Pakage() {

		}

		Pakage(const char* u) {
			url = u;
		}
		int getCount(const char* path) {
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("pakage") == 0) {
								count = count + 1;
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif //DEBUG
							}
						}
					}
				}
			}
			in.close();
			return count;
		}
		Pakage getFirstByKey(const char* path) {
			if (access(path, F_OK) != -1) {
			}
			else {
#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif //DEBUG
				return Pakage("ERR_READ_FILE");
			}
			std::ifstream in(path);
			std::string line;
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "" << value << "}" << std::endl;
#endif // DEBUG
							if (key.compare("pakage") == 0) {
								const char* _char = value.c_str();
								return Pakage(_char);
							}
						}
					}
				}
			}
		}
		Pakage* getAllByKey(const char* path) {
			std::ifstream in(path);
			std::string line;
			int count = 0;

			//Try to acess file
			if (access(path, F_OK) != -1) {
				//#ifdef DEBUG

				//#endif // DEBUG
			}
			else {

#ifdef DEBUG
				std::cout << "Error reading file" << std::endl;
#endif // DEBUG
				exit(1);
			}
			//read file
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							if (key.compare("pakage") == 0) {
								count = count + 1;
							}
						}
					}
				}
			}
			in.close();
			in.open(path);
			line = "";
			int i = 0;
			Pakage* values = new Pakage[count];
			//one more read
			while (std::getline(in, line))
			{
				if (!line.empty())
				{
					std::istringstream is_line(line);
					std::string key;
					if (std::getline(is_line, key, '='))
					{
						std::string value;
						if (std::getline(is_line, value)) {
							//Debug
#ifdef DEBUG
							std::cout << "Line{" << key << "=" << value << "}" << std::endl;
#endif // DEBUG
							//check if equels key
							if (key.compare("pakage") == 0) {
#ifdef DEBUG
								std::cout << "Compare sucess" << std::endl;
#endif // DEBUG
								values[i] = Pakage(value.c_str());
								i++;
							}
						}
					}
				}
			}
			in.close();
			return values;
		}
	};
#pragma endregion
#pragma region Spm_mirrorlist
	using namespace core;
	class MirrorList {
	public:
		Mirror* loadList() {
			const char* path = "/etc/spm/mirror.list";
			Mirror mirror;
			return mirror.getAllByKey(path);
		}
		
		void save(const char* url) {
			File file = File("/etc/spm/mirror.list");
			std::string conf = "server=";
			conf += url;

			file.appendLine(conf.c_str());

		}
		int removeMirror(const char* url) {
			core::File file = core::File("/etc/spm/mirror.list");
			std::string rLine = "server=";
			rLine += url;
			file.removeLine(rLine);
			return 0;

		}
		};

#pragma endregion
#pragma region Repository
	class Repository {
	public:
		int removeRepository(const char* url) {
			std::cout << "Removing " << url << " from mirror list" << "" << std::endl;
			MirrorList list;
			list.removeMirror(url);
		}
		int addRepository(const char* url) {
			//create http client
			httplib::Client cli(url);
			std::cout << "Requesting " << url << "/index" << "" << std::endl;
			//create resault object
			httplib::Result res;
			//request
			if (auto res = cli.Get("/index")) {
				//status code 200
				if (res->status == 200) {
					std::cout << "Ok" << std::endl;
					MirrorList list;
					list.save(url);
					
					return 0;
				}
				//if invalid status code
				std::cout << "Server returned:" << res->status << std::endl << "Repository not added!"<< std::endl;
				return 1;
			}
			//if http error
			else
			{
				auto err = res.error();
				std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
				return 1;
			}



		}
	};
#pragma endregion
#pragma region Update
	class Update {
	private:
		
		int downloadPakages() {
			remove("/var/cache/spm/pakages.list");
			Directory loader;
			Directory* dirsGetted = loader.getAllByKey("/var/cache/spm/directories.list");
			int count = loader.getCount("/var/cache/spm/directories.list");
			ProgressBar pkgBar = ProgressBar("Downloading pakages list", 1, count);
			for (int i = 0; i < count; i++)
			{
				std::vector<std::string> url = core::split(dirsGetted[i].url,'/');
				std::string cliUrl = "http://";
				cliUrl.append(url[0]);
				httplib::Client cli(cliUrl);
				httplib::Result res;
				File pakageCache = File("/var/cache/spm/pakages.list");
				File pakageListTmp = File("/tmp/spm/res_pkg_tmp");
				std::string geturl = "/";
				geturl.append(url[1]);
				if (url.size()-1 >= 2) {
					for (size_t i = 2; i < url.size(); i++)
					{
						geturl.append("/");geturl.append(url[i]);
					}
				}
				geturl.append("/index");
				//request
				if (auto res = cli.Get(geturl)) {
					//status code 200
					if (res->status == 200) {
						pakageListTmp.append(res->body);
						Pakage loader;
						int pkgCount = loader.getCount("/tmp/spm/res_pkg_tmp");
						Pakage* pkgs = loader.getAllByKey("/tmp/spm/res_pkg_tmp");
						for (size_t j = 0; j < pkgCount; j++)
						{
							std::string appLine = "pakage=";
							appLine.append(dirsGetted[i].url);
							appLine.append("/");
							appLine.append(pkgs[j].url);
							pakageCache.appendLine(appLine);
						}
					}else {
						std::cout << "Server " << url[0] << "request " << geturl << " returned:" << res->status << std::endl << "Error occured!" << std::endl;
						return 1;
					}
				}
				else
				{
					auto err = res.error();
					std::cout << "Server " << url[0] << "request " << geturl << ": HTTP error: " << httplib::to_string(err) << std::endl;
					return 1;
				}

				
				
				pkgBar.tick();
				remove("/tmp/spm/res_pkg_tmp");
			}
			
			
		}
	public:
		int update() {
			mkdir("/var/cache/spm", 0777);
			mkdir("/tmp/spm", 0777);
			using namespace core;
			File dirCache = File("/var/cache/spm/directories.list");
			dirCache.copyTo("/tmp/spm/res_dirs_tmp");
			remove("/var/cache/spm/directories.list");
			MirrorList loader;
			Mirror* mirrors = loader.loadList();
			int count = mirrors[0].getCount("/etc/spm/mirror.list");
			ProgressBar bar = ProgressBar("Downloading directories list", 1, count);
			for (size_t i = 0; i < count; i++)
			{
				httplib::Client cli(mirrors[i].url);
				httplib::Result res;
				//request
				if (auto res = cli.Get("/index")) {
					//status code 200
					if (res->status == 200) {
						mkdir("/var/cache/spm", 0777);
						mkdir("/tmp/spm", 0777);
						std::string tmp_file;
						File dir_file = File("/var/cache/spm/directories.list");
						File tmp_res = File("/tmp/spm/res_indexing_tmp");
						tmp_res.append(res->body);
						Directory loader;
						Directory* dirsGetted = loader.getAllByKey("/tmp/spm/res_indexing_tmp");
						int dirsGettedCount = loader.getCount("/tmp/spm/res_indexing_tmp");
						for (size_t j = 0; j < dirsGettedCount; j++)
						{
							
							dir_file.append("dir=");
							dir_file.append(mirrors[i].url);
							dir_file.append(dirsGetted[j].url);
							dir_file.append("\n");

						}
						bar.tick();
						remove("/tmp/spm/res_indexing_tmp");
					}
					else {
						std::cout << "Server " << mirrors[i].url << " returned:" << res->status << std::endl << "Error occured!" << std::endl;
						return 1;
					}
					
				}
				else
				{
					auto err = res.error();
					std::cout << "Server " << mirrors[i].url << ": HTTP error: " << httplib::to_string(err) << std::endl;
					return 1;
				}
			}
			remove("/tmp/spm/res_dirs_tmp");
			bar.reset();
			int retint =  downloadPakages();
			return retint;
		}
	};
#pragma endregion
#pragma region Install
	class Install {
	public:
		int remove(std::string pakage) {
			mkdir("/tmp/spm", 0777);
			mkdir("/var/log/spm", 0777);
			Pakage loader;
			std::cout << pakage;
			int pkgCount = loader.getCount("/var/cache/spm/pakages.list");
			Pakage* pkgs = loader.getAllByKey("/var/cache/spm/pakages.list");
			for (size_t j = 0; j < pkgCount; j++)
			{
				std::vector<std::string> strs = core::split(pkgs[j].url, ':');
				std::string pkgname = strs[1];
				std::string url = strs[0];
				std::vector<std::string> fnamestrs = core::split(url, '/');
				std::string filename = "/tmp/spm/";
				filename.append(fnamestrs[fnamestrs.size() - 1]);
				if (pkgname.compare(pakage) == 0) {
					std::cout << "Pakage found" << std::endl;
					std::string downUrl = "http://";
					downUrl.append(url);
					//std::cout << "Downloading " << (char*)downUrl.c_str() << std::endl;
					core::Downloader downloader = core::Downloader();
					bool sucess = downloader.download(downUrl, filename);
					if (!sucess) {
						std::cout << "Error occured while downloading pakage" << std::endl;
						return 1;
					}
					std::cout << "Extracting pakage" << std::endl;
					std::string targetDir = "/tmp/spm/";
					targetDir.append(pkgname);
					mkdir(targetDir.c_str(), 0777);
					std::string targetCmd = "tar -xzvf ";
					targetCmd += filename;
					targetCmd += " --directory ";
					targetCmd += targetDir;
					targetCmd += " >> /var/log/spm/spm-install-$(date '+%Y-%m-%d-%H:%M')";
					int status = system(targetCmd.c_str());
					if (status != 0) {
						std::cout << "Error occured while extracting pakage: Exit code " << status << std::endl;
						return 1;
					}
					std::cout << "Uninstalling pakage" << std::endl;
					std::string targetCmd2 = "sh ";
					targetCmd2 += targetDir;
					targetCmd2 += "/UNINSTALL ";
					targetCmd2 += targetDir;
					targetCmd2 += ">> /var/log/spm/spm-install-$(date '+%Y-%m-%d')";
					status = system(targetCmd2.c_str());
					if (status != 0) {
						std::cout << "Error occured while  installing: Exit code " << status << std::endl;
						return 1;
					}
					std::cout << "Sucessfully removed " << pkgname << " on your system\n";
					status = remove(targetDir.c_str());
					if (status != 0) {
						std::cout << "Error occured while removing tempory files: Exit code " << status << std::endl;
						return 1;
					}

					return 0;
				}
			}
		}

		int install(std::string pakage) {
			mkdir("/tmp/spm", 0777);
			mkdir("/var/log/spm", 0777);
			Pakage loader;
			std::cout << pakage;
			int pkgCount = loader.getCount("/var/cache/spm/pakages.list");
			Pakage* pkgs = loader.getAllByKey("/var/cache/spm/pakages.list");
			for (size_t j = 0; j < pkgCount; j++)
			{
				std::vector<std::string> strs = core::split(pkgs[j].url, ':');
				std::string pkgname = strs[1];
				std::string url = strs[0];
				std::vector<std::string> fnamestrs = core::split(url, '/');
				std::string filename = "/tmp/spm/";
				filename.append(fnamestrs[fnamestrs.size() - 1]);
				if (pkgname.compare(pakage) == 0) {
					std::cout << "Pakage found" << std::endl;
					std::string downUrl = "http://";
					downUrl.append(url);
						//std::cout << "Downloading " << (char*)downUrl.c_str() << std::endl;
						core::Downloader downloader = core::Downloader();
						bool sucess = downloader.download(downUrl, filename);
						if (!sucess) {
							std::cout << "Error occured while downloading pakage" << std::endl;
							return 1;
						}
						std::cout << "Extracting pakage" << std::endl;
						std::string targetDir = "/tmp/spm/";
						targetDir.append(pkgname);
						mkdir(targetDir.c_str(), 0777);
						std::string targetCmd = "tar -xzvf ";
						targetCmd += filename;
						targetCmd += " --directory ";
						targetCmd += targetDir;
						targetCmd += " >> /var/log/spm/spm-install-$(date '+%Y-%m-%d-%H:%M')";
						int status = system(targetCmd.c_str());
						if (status != 0) {
							std::cout << "Error occured while extracting pakage: Exit code " << status << std::endl;
							return 1;
						}
						std::cout << "Installing dependencies" << std::endl;
						std::string dependsPath = targetDir + "/DEPENDS";
						File instPkgFile = File("/etc/spm/installed.list");
						File file = File(dependsPath.c_str());
						std::string fileContent = file.readAllText();
						auto depends = core::split(fileContent, ':');
						int length = depends.size();
						std::string end = "end";
						for (size_t i = 0; i < length; i++)
						{
							if (depends[i].compare(end) != 0) {
								int status = this->install(depends[i]);
								if (status != 0) {
									std::cout << "Failed to install " << depends[i] << std::endl;
									return 1;
								}
							}
							
						}
						std::cout << "Installing pakage" << std::endl;
						std::string targetCmd2 = "sh ";
						targetCmd2 += targetDir;
						targetCmd2 += "/PKGBUILD ";
						targetCmd2 += targetDir;
						targetCmd2 += ">> /var/log/spm/spm-install-$(date '+%Y-%m-%d')";
						status = system(targetCmd2.c_str());
						if (status != 0) {
							std::cout << "Error occured while  installing: Exit code " << status << std::endl;
							return 1;
						}
						std::cout << "Sucessfully installed " << pkgname << " on your system\n";
						status = remove(targetDir.c_str());
						if (status != 0) {
							std::cout << "Error occured while removing tempory files: Exit code " << status << std::endl;
							return 1;
						}
						
						return 0;
					}
				}
			std::cout << "Coud not find pakage" << std::endl;
			return 1;
		}
	};
#pragma endregion
	int add_repository(const char* url) {
		mkdir("/var/cache/spm", 0777);
		mkdir("/tmp/spm", 0777);
		mkdir("/etc/spm", 0777);
		Repository adder;
		return adder.addRepository(url);
	}
	int remove_repository(const char* url) {
		Repository adder;
		return adder.removeRepository(url);
	}
	int update() {
		Update update;
		return update.update();
	}
	int install(const char* pkgname) {
		Install installer;
		return installer.install(pkgname);
	}
	int remove(const char* pkgname) {
		Install installer;
		return installer.remove(pkgname);
	}
};

