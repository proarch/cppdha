cmake_minimum_required(VERSION 3.10)

project(cppdha)

set (CMAKE_CXX_STANDARD_LIBRARIES "-lcryptopp -lcurl -pthread -lssl -lcrypto")

include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(/usr/include/cryptopp)
include_directories(includes)
include_directories(src)
include_directories(headers)

file(GLOB SOURCE_CPP
    src/*.cpp
    ./*.cpp
)

add_library(cppdha ${SOURCE_CPP})
#add_executable (cppdha sample.cpp ${SOURCE_CPP})
target_compile_features(cppdha PRIVATE cxx_std_17)