

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# List all source files in the 'commands' folder
file(GLOB DATASTRUCTS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(DataStructsLib ${DATASTRUCTS_SOURCES})

target_include_directories(DataStructsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)