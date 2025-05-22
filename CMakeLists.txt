cmake_minimum_required(VERSION 3.18)

project(Bindle)

include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Boost REQUIRED)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)

# Generate gRPC sources
# Adapted from https://github.com/grpc/grpc/blob/v1.72.0/examples/cpp/route_guide/CMakeLists.txt
get_target_property(PROTOC_PATH protobuf::protoc IMPORTED_LOCATION)
get_target_property(GRPC_CPP_PLUGIN_PATH gRPC::grpc_cpp_plugin IMPORTED_LOCATION)

get_filename_component(bindle_proto "./bindle.proto" ABSOLUTE)
get_filename_component(bindle_proto_path "${bindle_proto}" PATH)

set(bindle_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/bindle.pb.cc")
set(bindle_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bindle.pb.h")
set(bindle_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/bindle.grpc.pb.cc")
set(bindle_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bindle.grpc.pb.h")
add_custom_command(
      OUTPUT "${bindle_proto_srcs}" "${bindle_proto_hdrs}" "${bindle_grpc_srcs}" "${bindle_grpc_hdrs}"
      COMMAND ${PROTOC_PATH}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${bindle_proto_path}"
	--plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_PATH}"
        "${bindle_proto}"
      DEPENDS "${bindle_proto}")

add_library(bindle_grpc_proto
	${bindle_grpc_srcs}
	${bindle_grpc_hdrs}
	${bindle_proto_srcs}
	${bindle_proto_hdrs})
target_include_directories(bindle_grpc_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bindle_grpc_proto PUBLIC gRPC::grpc++ protobuf::libprotobuf)

add_executable(${PROJECT_NAME} src/grpc_server.cpp src/db.cpp)
target_link_libraries(${PROJECT_NAME}
	Boost::boost
	bindle_grpc_proto)


add_executable(test_client src/test_client.cpp)
target_link_libraries(test_client
	bindle_grpc_proto)
