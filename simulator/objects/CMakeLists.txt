

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# List all source files in the 'commands' folder
file(GLOB OBJECTS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
# add_library(ObjectsLib ${OBJECTS_SOURCES})

# target_include_directories(ObjectsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Add a library or target specific to this folder
# Example: 
# add_library(commands STATIC ${COMMANDS_SOURCES})

# Link libraries or set properties for this specific module
# target_link_libraries(commands PRIVATE ${wxWidgets_LIBRARIES})
