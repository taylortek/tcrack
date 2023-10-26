#include <openssl/md5.h>
#include <openssl/sha.h>
#include <iomanip>
#include <vector>
#include <functional>
#include <map>

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

std::string sha256(const std::string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
	std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string sha1(const std::string str)
{
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX sha1;
    SHA1_Init(&sha1);
    SHA1_Update(&sha1, str.c_str(), str.size());
    SHA1_Final(hash, &sha1);
	std::stringstream ss;
    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string hashIdentify(std::string hash) {
	switch(hash.size()){
		case 32:
			return "md5";
		case 40:
			return "sha1";
		case 64:
			return "sha256";
		default:
			std::cerr << "\033[u\033[KCould not identify hash...\n";
			std::exit(-1);
	}
}

std::function<std::string(std::string)> hashSet(std::string hash_name) {
	std::map<std::string, std::function<std::string(std::string)>> hash_map;
	hash_map["md5"] = md5;
	hash_map["sha256"] = sha256;
	hash_map["sha1"] = sha1;
	if (!hash_map.count(hash_name)) {
		std::cerr << "\033[u\033[KNo hash \"" << hash_name << "\" found\n";
		std::exit(-1);
	}
	return hash_map[hash_name];
}
