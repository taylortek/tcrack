#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
#include <openssl/md5.h>
#include <iomanip>
#include <vector>

int getSize(std::ifstream &file) {
	int reset = file.tellg();
	int size = file.seekg(0, std::ios::end).tellg();
	file.seekg(reset, std::ios::beg);
	return size;
}

std::string md5(const std::string &str) {
    unsigned char hash[MD5_DIGEST_LENGTH];

    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    std::stringstream ss;

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

void crack(std::string hash, std::stringstream &ss) {
	std::string word;
	while (!ss.eof()) {
		ss >> word;
		if (md5(word) == hash) {
			std::cout << "\033[u\033[K" << hash << " : \e[41m"<< word << "\e[0m\n";
			std::exit(0);
		}
	}
}

int main(int argc, char** argv) {

	if (argc < 4) {
		std::cout << "./tcrack <hash> /path/to/wordlist <threads>\n";
		std::exit(-1);
	}

	std::cout << "\e[1m" << R"(

  _____  taytek presents  _    
 |_   _|__ _ __ __ _  ___| | __
   | |/ __| '__/ _` |/ __| |/ /
   | | (__| | | (_| | (__|   < 
   |_|\___|_|  \__,_|\___|_|\_\
)" << "\e[0m\n";

	std::cout << "\e[1;5;31m   !POTENTIALLY MEMORY UNSAFE!\e[0m\n\n\033[sThis is a work in progress. Continue? y or n: ";
	
	std::string confirm;
	std::cin >> confirm;
	if (confirm != "y") {
		std::exit(-1);
	}

	std::cout << "\033[u\033[KSplitting into " << argv[3] << " threads... (this will use a lot of ram)\n";

	int amount = std::stoi(argv[3]);
	std::ifstream wordlist;
	wordlist.open(argv[2]);
	std::string hash = argv[1];
	std::string word;

	int size = 0;
	
	while (wordlist.good()) {
		wordlist >> word;
		size++;
	}

	wordlist.clear();
	wordlist.seekg(0, std::ios::beg);

	std::vector<std::stringstream> stream_vector;
	std::vector<std::thread> thread_vector;

	for (int i=0; i<amount; i++) {
		stream_vector.push_back(std::stringstream());
		for (int x=0; x<size/amount; x++) {
			wordlist >> word;
			stream_vector.at(i) << word << "\n";
		}
	}

	while (wordlist.good()) {
		wordlist >> word;
		stream_vector.at(amount-1) << word << "\n";
	}

	std::cout << "\033[u\033[KArmed and starting... (your ram is stable now)\n";

	for (int i=0; i<amount; i++) {
		thread_vector.push_back(std::thread(crack, std::ref(hash), std::ref(stream_vector.at(i))));
	}

	for (int i=0; i<amount; i++) {
		thread_vector.at(i).join();
	}
	
	wordlist.close();

	std::cout << "\033[u\033[KBetter luck next time...\n";

	return 0;
}
