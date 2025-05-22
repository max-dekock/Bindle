#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <iostream>
#include <string>
#include <memory>
#include <utility>

#include "bindle.grpc.pb.h"

#include "db.hpp"

using grpc::Status;
using grpc::Server;
using grpc::ServerContext;
using grpc::ServerBuilder;
using bindle::Bindle;
using bindle::Key;
using bindle::KeyValue;
using bindle::GetResult;
using bindle::InsertResult;
using bindle::RemoveResult;

class BindleServerImpl final : public Bindle::Service {
public:
	explicit BindleServerImpl(Database db) : db_(std::move(db)) {}

	Status GetValue(ServerContext* context, const Key* key, GetResult* result) {
		std::cout << "Getting value for key " << key->key() << std::endl;
		auto value = db_.get(key->key());
		if (value) {
			result->set_value(*value);
		}
		return Status::OK;
	}

	Status InsertValue(ServerContext* context, const KeyValue* kv, InsertResult* result) {
		std::cout << "Inserting value " << kv->value() << " for key " << kv->key() << std::endl;
		db_.insert(kv->key(), kv->value());
		return Status::OK;
	}

	Status RemoveKey(ServerContext* context, const Key* key, RemoveResult* result) {
		std::cout << "Removing key " << key->key() << std::endl;
		db_.remove(key->key());
		return Status::OK;
	}

private:
	Database db_;
};

int main() {
	Database db("db.bin");

	db.insert("Foo", "Hello, world! For real this time!");

	std::string server_address("0.0.0.0:50051");
	BindleServerImpl service(std::move(db));

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();

	return 0;
}
