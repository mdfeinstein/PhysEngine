

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# List all source files in the 'commands' folder
file(GLOB RECORDING_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
add_library(RecordingLib ${RECORDING_SOURCES})

target_include_directories(RecordingLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Add a library or target specific to this folder
# Example: 
# add_library(commands STATIC ${COMMANDS_SOURCES})

# Link libraries or set properties for this specific module
# target_link_libraries(commands PRIVATE ${wxWidgets_LIBRARIES})
