cmake_minimum_required(VERSION 3.14)
project(PhysEngineExample LANGUAGES CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify C++ compiler
set(CMAKE_CXX_COMPILER "C:/msys64/ucrt64/bin/g++.exe")

# wxWidgets setup
set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.2.6/install")
set(wxWidgets_INCLUDE_DIR "${wxWidgets_ROOT_DIR}/include")
set(wxWidgets_LIBRARY_DIR "${wxWidgets_ROOT_DIR}/lib")

set(wxWidgets_LIBRARIES
  ${wxWidgets_LIBRARY_DIR}/gcc_x64_dll/libwxbase32ud.a
  ${wxWidgets_LIBRARY_DIR}/gcc_x64_dll/libwxmsw32ud_core.a
)

# Python setup
set(PYTHON_EXECUTABLE "C:/Users/Mati/miniconda3/envs/myenv/python.exe")
set(PYTHON_INCLUDE_DIR "C:/Users/Mati/miniconda3/envs/myenv/include")
set(PYTHON_LIBRARY "C:/Users/Mati/miniconda3/envs/myenv/libs/python39.lib")

# Set up pybind11
set(PYBIND11_DIR "C:/Users/Mati/miniconda3/envs/myenv/Library/share/cmake/pybind11")
find_package(pybind11 REQUIRED CONFIG PATHS ${PYBIND11_DIR})

# Include directories
include_directories(
  ${wxWidgets_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  "${CMAKE_SOURCE_DIR}/"
  "${CMAKE_SOURCE_DIR}/simulator"
  "${CMAKE_SOURCE_DIR}/simulator/commands"
  "${CMAKE_SOURCE_DIR}/simulator/dataStructs"
  "${CMAKE_SOURCE_DIR}/simulator/effects"
  "${CMAKE_SOURCE_DIR}/simulator/interactions"
  "${CMAKE_SOURCE_DIR}/simulator/interactions/interactionTypes"
  "${CMAKE_SOURCE_DIR}/simulator/Effects"
  "${CMAKE_SOURCE_DIR}/simulator/Effects/EffectTypes"
  "${CMAKE_SOURCE_DIR}/simulator/movers"
  "${CMAKE_SOURCE_DIR}/simulator/objects"
  "${CMAKE_SOURCE_DIR}/simulator/recording"
  "${CMAKE_SOURCE_DIR}/simulator/utility"
  "${CMAKE_SOURCE_DIR}/gui"
  "${CMAKE_SOURCE_DIR}/gui/controls"
  "${CMAKE_SOURCE_DIR}/gui/visuals"
  "${CMAKE_SOURCE_DIR}/gui/utility"
  "${CMAKE_SOURCE_DIR}/pybind"
  
)


# Add subdirectories
add_subdirectory(simulator)
add_subdirectory(gui)
add_subdirectory(pybind)

set(ALL_LIBRARIES 
  SimulatorLib
  # GuiLib
  # pybindLib
  )

# add_subdirectory(apps/pybindApp)
# add_subdirectory(apps/standalone1)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

enable_testing()

add_test(NAME Vect2_Test COMMAND Vect2_test)
add_test(NAME Mover_Test COMMAND Mover_test)
add_test(NAME MoverFactory_Test COMMAND MoverFactory_test)
add_test(NAME RigidMover_Test COMMAND RigidMover_test)
add_test(NAME Simulator_Test COMMAND Simulator_test)
add_test(NAME Effect_Test COMMAND Effect_test)
# gtest_discover_tests(Vect2_test
#     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
# )