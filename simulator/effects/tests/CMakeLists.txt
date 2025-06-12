cmake_minimum_required(VERSION 3.14)
set( CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe" )
set( CMAKE_C_COMPILER "C:/msys64/ucrt64/bin/gcc.exe" )
set(CMAKE_GENERATOR "MinGW Makefiles") 
project(Effect_test)

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

# List of test source files
set(TEST_SOURCES
  Effect_test.cpp
)

# Iterate over each test source file and create a test executable
foreach(TEST_SOURCE ${TEST_SOURCES})
  # Extract the test name without the file extension
  get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
  
  add_executable(${TEST_NAME} ${TEST_SOURCE})
  
  target_link_libraries(${TEST_NAME}
    GTest::gtest_main
    # EffectsLib
    DataStructsLib
    MoversLib
  )
  
  set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

  # Register test with CTest
  gtest_discover_tests(${TEST_NAME})
endforeach() 