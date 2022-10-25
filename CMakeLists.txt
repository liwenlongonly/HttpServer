cmake_minimum_required(VERSION 3.8)

project(HttpServer)

set(CMAKE_CXX_STANDARD 14)

set(LIB_DIR ${PROJECT_SOURCE_DIR}/workflow/lib)

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${LIB_DIR} ./lib)

include_directories(${PROJECT_SOURCE_DIR}/workflow/include)
include_directories(${PROJECT_SOURCE_DIR}/*.h)

link_directories(${LIB_DIR})

file(GLOB _Source ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.c)

find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)

set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

set(TARGET_NAME HttpServer)

add_executable(${TARGET_NAME} ${_Source})
target_link_libraries(${TARGET_NAME} ${WORKFLOW_LIB})

set(install_DIR ${PROJECT_SOURCE_DIR}/${TARGET_NAME}_install)

install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${install_DIR}
        )

install(DIRECTORY ${LIB_DIR} DESTINATION ${install_DIR}
        PATTERN "CVS" EXCLUDE # 剔除文件
        )

#install(
#        FILES README.md
#        DESTINATION "${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION}"
#        COMPONENT devel
#       )
