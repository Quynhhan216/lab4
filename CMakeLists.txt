cmake_minimum_required(VERSION 3.16)
project(lab4_hashing_pki CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hashtool
    src/main.cpp
    src/hash_engine.cpp
    src/kat_runner.cpp
)

find_library(CRYPTOPP_LIB cryptopp)
if(NOT CRYPTOPP_LIB)
    message(FATAL_ERROR "Khong tim thay thu vien Crypto++ (cryptopp). Hay cai libcrypto++-dev.")
endif()

target_link_libraries(hashtool PRIVATE ${CRYPTOPP_LIB})

# --- Unit test (Catch2 ban amalgamated) ---
enable_testing()

add_executable(hashtool_tests
    tests/test_hashtool.cpp
    tests/catch_amalgamated.cpp
    src/hash_engine.cpp
)
target_include_directories(hashtool_tests PRIVATE src)
target_link_libraries(hashtool_tests PRIVATE ${CRYPTOPP_LIB})

add_test(NAME hashtool_unit_tests COMMAND hashtool_tests)
