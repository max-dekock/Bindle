#include "db.hpp"

#include <iostream>

using namespace boost::interprocess;

Database::Database(const char* filename) : mfile_(open_or_create, filename, 1048576) {}

void Database::insert(std::string key, std::string value) {
	remove(key);
	mfile_.construct_it<char>(key.c_str())[value.size()](value.begin());
}

boost::optional<std::string> Database::get(const std::string& key) {
	auto [ ptr, size ] = mfile_.find<char>(key.c_str());
	if (ptr) {
		return std::string(ptr, size);
	} else {
		return boost::none;
	}
}

void Database::remove(std::string key) {
	mfile_.destroy<char>(key.c_str());
}

