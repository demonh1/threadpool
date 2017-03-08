project(threadpool)
cmake_minimum_required(VERSION 2.8)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    #set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
aux_source_directory(. SRC_LIST)
set(SRC_LIST ${SRC_LIST} "tasks.h" "worker.h")

# boost
find_package(Boost 1.54.0 COMPONENTS system thread filesystem REQUIRED)
if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif ()
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})
