#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

//#include "boost/optional.hpp"

#include <iostream>
#include <string>
#include <memory>

#include "bindle.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using namespace bindle;

class BindleClient {
public:
	BindleClient(std::shared_ptr<Channel> channel) : stub_(Bindle::NewStub(channel)) {}

	void GetValue(const Key& key) {
		ClientContext context;
		GetResult result;
		Status status = stub_->GetValue(&context, key, &result);
		if (!status.ok()) {
			std::cout << "GetValue rpc failed." << std::endl;
			return;
		}
		if (result.has_value()) {
			std::cout << result.value() << std::endl;
		} else {
			std::cout << "[no value]" << std::endl;
		}
	}

	void InsertValue(const KeyValue& kv) {
		ClientContext context;
		InsertResult result;
		Status status = stub_->InsertValue(&context, kv, &result);
		if (!status.ok()) {
			std::cout << "InsertValue rpc failed." << std::endl;
		}
	}

	void RemoveKey(const Key& key) {
		ClientContext context;
		RemoveResult result;
		Status status = stub_->RemoveKey(&context, key, &result);
		if (!status.ok()) {
			std::cout << "RemoveKey rpc failed." << std::endl;
		}
	}

private:
	std::unique_ptr<Bindle::Stub> stub_;
};


int main() {
	BindleClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));

	while (1) {
		std::cout << "(1) get (2) insert (3) delete" << std::endl;
		std::string choice_str;
		std::getline(std::cin, choice_str);
		int choice = std::stoi(choice_str);

		std::string key_str;
		std::string val_str;
		Key key;
		KeyValue kv;
		switch (choice) {
			case 1:
				std::getline(std::cin, key_str);
				key.set_key(key_str);
				client.GetValue(key);
				break;
			case 2:
				std::getline(std::cin, key_str);
				std::getline(std::cin, val_str);
				kv.set_key(key_str);
				kv.set_value(val_str);
				client.InsertValue(kv);
				break;
			case 3:
				std::getline(std::cin, key_str);
				key.set_key(key_str);
				client.RemoveKey(key);
				break;
			default:
				std::cout << "Invalid choice" << std::endl;
		}
	}
}
