cmake_minimum_required(VERSION 3.14)
set( CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe" )
set( CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe" )
set(CMAKE_GENERATOR "MinGW Makefiles") 
project(Vect2_test)


# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(
  Vect2_test
  Vect2_test.cpp
)
target_link_libraries(
  Vect2_test
  GTest::gtest_main
  DataStructsLib
)

include(GoogleTest)
# gtest_discover_tests(Vect2_test)
set_target_properties(Vect2_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
gtest_discover_tests(Vect2_test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    DISCOVERY_TIMEOUT 10
)
# add_test(NAME Vect2_Test COMMAND Vect2_test)