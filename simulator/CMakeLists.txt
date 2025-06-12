# simulator/CMakeLists.txt

# Add subdirectories for various modules in the simulator
add_subdirectory(commands)
add_subdirectory(dataStructs)
add_subdirectory(effects)
add_subdirectory(interactions)
add_subdirectory(movers)
add_subdirectory(objects)
# add_subdirectory(recording)
add_subdirectory(utility)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Define a list of sources specific to the simulator
file(GLOB SIMULATOR_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(SimulatorLib ${SIMULATOR_SOURCES})
target_include_directories(SimulatorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(SimulatorLib PUBLIC
  DataStructsLib
  InteractionsLib
  MoversLib
  # ObjectsLib
  # EffectsLib
  # UtilityLib
  # RecordingLib
  # CommandsLib
)
add_subdirectory(tests)

# Optional: Create a library or other targets for the simulator if needed
# Example: 
# add_library(simulator_lib STATIC ${SIMULATOR_SOURCES})

# Link libraries if needed (e.g., wxWidgets)
# target_link_libraries(simulator_lib PRIVATE ${wxWidgets_LIBRARIES})
