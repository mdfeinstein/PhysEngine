# pybind/CMakeLists.txt

# Here, you would configure any source files or dependencies specific to pybind
file(GLOB PYBIND_SOURCES
  "${CMAKE_SOURCE_DIR}/pybind/*.cpp"
)

# add_library(pybindLib ${PYBIND_SOURCES})

# target_include_directories(pybindLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add other pybind-specific configurations if needed
# Example: set_target_properties, link libraries, etc.
