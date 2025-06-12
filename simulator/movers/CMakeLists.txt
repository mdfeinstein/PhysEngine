

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# List all source files in the 'commands' folder
file(GLOB MOVERS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
add_library(MoversLib ${MOVERS_SOURCES})

target_include_directories(MoversLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)
# Add a library or target specific to this folder
# Example: 
# add_library(commands STATIC ${COMMANDS_SOURCES})

# Link libraries or set properties for this specific module
# target_link_libraries(commands PRIVATE ${wxWidgets_LIBRARIES})
