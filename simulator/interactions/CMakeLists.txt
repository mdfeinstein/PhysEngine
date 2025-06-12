

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/InteractionTypes
  )
# List all source files in the 'commands' folder
file(GLOB INTERACTIONS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/InteractionTypes/*.cpp"
)

add_library(InteractionsLib ${INTERACTIONS_SOURCES})

target_include_directories(InteractionsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# message(STATUS "INTERACTIONS_SOURCES: ${INTERACTIONS_SOURCES}")

# Add a library or target specific to this folder
# Example: 
# add_library(commands STATIC ${COMMANDS_SOURCES})

# Link libraries or set properties for this specific module
# target_link_libraries(commands PRIVATE ${wxWidgets_LIBRARIES})
