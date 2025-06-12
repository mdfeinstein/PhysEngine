

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# List all source files in the 'commands' folder
file(GLOB EFFECTS_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/EffectTypes/*.cpp"
)

# add_library(EffectsLib ${EFFECTS_SOURCES})

# target_include_directories(EffectsLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(tests)