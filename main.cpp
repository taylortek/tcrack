#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
#include <vector>
#include "hashing.h"

int getSize(std::ifstream &file) {
	int reset = file.tellg();
	int size = file.seekg(0, std::ios::end).tellg();
	file.seekg(reset, std::ios::beg);
	return size;
}

void crack(std::string hash, std::stringstream &ss, std::function<std::string(std::string)> hash_mode) {
	std::string word;
	while (!ss.eof()) {
		ss >> word;
		if (hash_mode(word) == hash) {
			std::cout << "\033[u\033[K" << hash << " : \e[41m"<< word << "\e[0m\n";
			std::exit(0);
		}
	}
}

int main(int argc, char** argv) {
	if (argc < 4) {
		std::cout << "./tcrack <hash> /path/to/wordlist <threads> (optional hash type)\n\n";
		std::cout << R"(Formats:
	md5
	sha256
	sha512
	sha1)" << "\n";
		std::exit(-1);
	}

	std::string hash = argv[1];
	std::string word;

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

	std::function<std::string(std::string)> hash_mode;
	std::string hash_name;

	if (argc > 4) {
		hash_name = argv[4];
	} else {
		hash_name = hashIdentify(hash);
	}

	hash_mode = hashSet(hash_name);

	std::cout << "\033[u\033[KUsing \"" << hash_name << "\" mode...\n\033[s";

	std::cout << "\033[u\033[KSplitting into " << argv[3] << " threads... (this will use a lot of ram)\n";

	int amount = std::stoi(argv[3]);
	std::ifstream wordlist;
	wordlist.open(argv[2]);
	if (wordlist.fail()) {
		std::cerr << "\033[u\033[KCould not open file " << argv[2] << "\n";
		std::exit(1);
	}

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
		thread_vector.push_back(std::thread(crack, std::ref(hash), std::ref(stream_vector.at(i)), std::ref(hash_mode)));
	}

	for (int i=0; i<amount; i++) {
		thread_vector.at(i).join();
	}
	
	wordlist.close();

	std::cout << "\033[u\033[KBetter luck next time...\n";

	return 0;
}
