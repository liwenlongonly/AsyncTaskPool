
cmake_minimum_required(VERSION 3.0.0)

project(async_task_pool)

message(STATUS "Project: SERVER")          #打印相关消息消息
message(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

#supports c++11
include(CheckCXXCompilerFlag)  
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)  
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)  
if(COMPILER_SUPPORTS_CXX11)  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")  
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} and flag -std=c++11")  
elseif(COMPILER_SUPPORTS_CXX0X)  
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")  
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} and flag -std=c++0x")  
else()  
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.Please use a different C++ compiler.")  
endif() 

# Specifies a path to native header files.
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB native_srcs "*.h" "*.cpp" )

message(STATUS "native_srcs: ${native_srcs}")

#config source group
source_group(asynctaskpool FILES ${native_srcs} )

add_library( # Sets the name of the library.
             async_task_pool

             # Sets the library as a shared library.
             STATIC

             # Provides a relative path to your source file(s).
             ${native_srcs} )
