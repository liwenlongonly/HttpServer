cmake_minimum_required(VERSION 3.0)

project(HttpServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/workflow/include)
include_directories(${PROJECT_SOURCE_DIR}/*.h)

link_directories(${PROJECT_SOURCE_DIR}/workflow/lib)

file(GLOB _Source ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.c)

find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)

set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

add_executable(HttpServer ${_Source})
target_link_libraries(HttpServer ${WORKFLOW_LIB})

