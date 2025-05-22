#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <boost/interprocess/exceptions.hpp>

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
		auto value = db_.get(key->key());
		if (value) {
			result->set_value(*value);
		}
		return Status::OK;
	}

	Status InsertValue(ServerContext* context, const KeyValue* kv, InsertResult* result) {
		try {
			db_.insert(kv->key(), kv->value());
			return Status::OK;
		} catch (const boost::interprocess::bad_alloc& e) {
			std::cerr << "Failed to insert: out of space: " << e.what() << std::endl;
			return Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "Out of memory");
		}
	}

	Status RemoveKey(ServerContext* context, const Key* key, RemoveResult* result) {
		db_.remove(key->key());
		return Status::OK;
	}

private:
	Database db_;
};

int main(int argc, char **argv) {
	const char *db_filename;
	if (argc == 2) {
		db_filename = argv[1];
	} else if (argc == 1) {
		db_filename = "db.bin";
	} else {
		std::cerr << "Usage: " << argv[0] << "[filename]" << std::endl;
		return 1;
	}
	Database db(db_filename);

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
