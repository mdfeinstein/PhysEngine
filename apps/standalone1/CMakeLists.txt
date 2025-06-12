# Specify the pybind module target
project(stndalone1App LANGUAGES CXX)

#include path of this directory 
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Set the path to the source files for the pybind module
file(GLOB APP_SOURCES "mainEntry.cpp")

# Create the pybind module
add_executable(standalone1 ${APP_SOURCES})

# Set the output directory for the pybind module
set_target_properties(standalone1 PROPERTIES 
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output"
)

# Link wxWidgets, Python libraries, and Simulator libraries
target_link_libraries(standalone1 PRIVATE
  ${wxWidgets_LIBRARIES}
  ${ALL_LIBRARIES}
)
