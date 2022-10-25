cmake_minimum_required(VERSION 3.8)

project(HttpServer)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${PROJECT_SOURCE_DIR}/workflow/lib ./lib)

include_directories(${PROJECT_SOURCE_DIR}/workflow/include)
include_directories(${PROJECT_SOURCE_DIR}/*.h)

link_directories(${PROJECT_SOURCE_DIR}/workflow/lib)

file(GLOB _Source ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.c)

find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)

set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

add_executable(HttpServer ${_Source})
target_link_libraries(HttpServer ${WORKFLOW_LIB})

