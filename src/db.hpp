#ifndef DB_H
#define DB_H

#include <map>
#include <string>

#include <boost/optional.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>

using namespace boost::interprocess;

class Database
{
public:
	Database(const char* filename);

	void insert(std::string key, std::string value);

	boost::optional<std::string> get(const std::string& key);

	void remove(std::string key);

private:
	managed_mapped_file mfile_;
};

#endif // #ifndef DB_H
