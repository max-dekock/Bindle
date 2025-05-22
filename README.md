# Bindle

A key-value store with all the elegance of a bag on a stick.

## Features

- Keys and values are arbitrary strings.
- Supports get, insert, and remove operations.
- Data is persisted on disk.
- Communicates via gRPC, allowing you to easily generate client code
  in any language.
- Supports multiple concurrent clients.

## Build Instructions

I have provided pre-build binaries for Windows and Linux. In case you 
want to build from source, you will require CMake and Conan.

On Windows:
```
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

On Linux:
```
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
cmake --build .
```

## Usage

```
.\Path\To\Bindle [filename]
```

If no database filename is specified, it defaults to `db.bin`. If the
file doesn't exist, it is created and initialized as an empty database.
The server listens for incoming requests on port 50051.

## How do I send data to the service? How do I receive data from the service?

Bindle uses gRPC to send and receive data from clients. gRPC is a remote
procedure protocol where services have their communication contracts defined
in `.proto` files. Clients and servers both use these `.proto` files to
automatically generate code to send and receive data in accordance with the
contract.

Bindle's communication contract is defined in `bindle.proto`. To write a
client program for bindle,
[follow the tutorial for your preferred language](https://grpc.io/docs/languages/).

### Example: Writing a Python Client

First download the `bindle.proto` file from this reposity, and add it to your 
project.

Next, install the grpcio-tools package, and use it to generate Python code
for the client:
```
pip install grpcio-tools
python -m grpc_tools.protoc --python_out=. --pyi_out=. --grpc_python_out=. bindle.proto
```

In your Python program, you will need to import these modules:
```
import grpc
import bindle_pb2
import bindle_pb2_grpc
```

Then you can create a "stub" to call Bindle's service methods:
```
channel = grpc.insecure_channel("localhost:50051")
stub = bindle_pb2_grpc.BindleStub(channel)
```

The stub can call all the methods defined in `bindle.proto`.
```
kv = bindle_pb2.KeyValue(key="Foo", value="Hello, world!")
key = bindle_pb2.Key(key="Foo")

# insert a key-value pair
stub.InsertValue(kv)

# get a previously inserted value
result = stub.GetValue(key)
print(result.value) # Hello, world!

# remove a key-value pair
stub.RemoveKey(key)
```

Note that these methods may throw an exception if the method call fails for
whatever reason.

### UML Sequence Diagram

[TODO]

