#include <iostream>
#include <string>
#include <map>

#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

class Database
{
	std::map<std::string, std::string> store;

public:
	void insert(std::string key, std::string value)
	{
		store[key] = value;
	}

	boost::optional<std::string> get(std::string& key)
	{
		if (auto result = store.find(key); result != store.end()) {
			return result->second;
		} else {
			return boost::none;
		}
	}

	void remove(std::string key) {
		store.erase(key);
	}

	
};

enum class RequestType
{
	insert,
	get,
	remove
};

struct Request
{
	RequestType request_type;
	std::string key;
	boost::optional<std::string> value;

	static boost::optional<Request> parse(const std::string& message)
	{
		// tokenize
		std::vector<std::string> tokens;
		boost::split(tokens, message, boost::is_any_of(":"));

		// find request type
		RequestType request_type;
		std::string& request_type_token = tokens[0];
		if (request_type_token == "insert") {
			request_type = RequestType::insert;
		} else if (request_type_token == "get") {
			request_type = RequestType::get;
		} else if (request_type_token == "remove") {
			request_type = RequestType::remove;
		} else {
			// invalid request type
			return boost::none;
		}

		// validate number of tokens
		size_t expected_num_tokens = 2 + (request_type == RequestType::insert);
		if (tokens.size() != expected_num_tokens) {
			return boost::none;
		}

		std::string key = tokens[1];
		boost::optional<std::string> value;
		if (request_type == RequestType::insert) {
			value = tokens[2];
		}

		return Request { request_type, key, value };
	}
};


int main(int argc, char** argv)
{
	using namespace boost::interprocess;

	std::string input;
	std::cin >> input;
	boost::optional<Request> req = Request::parse(input);
	if (req) {
		std::cout << "Request type: " << (int) req->request_type << std::endl;
		std::cout << "Key: " << req-> key << std::endl;
		if (req->value) {
			std::cout << "Value: " << *req->value << std::endl;
		}
	} else {
		std::cout << "Invalid request" << std::endl;
	}

	return 0;
}
