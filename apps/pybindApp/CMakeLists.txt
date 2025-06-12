# Specify the pybind module target
project(PybindApp LANGUAGES CXX)

#include path of this directory 
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Set the path to the source files for the pybind module
file(GLOB PYBINDAPP_SOURCES "pybinder.cpp")

# Create the pybind module
pybind11_add_module(phys_engine ${PYBINDAPP_SOURCES})

# Set the output directory for the pybind module
set_target_properties(phys_engine PROPERTIES 
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/apps/pybindApp"
)

# Link wxWidgets, Python libraries, and Simulator libraries
target_link_libraries(phys_engine PRIVATE
  ${wxWidgets_LIBRARIES}
  ${PYTHON_LIBRARY}
  ${ALL_LIBRARIES}
)
