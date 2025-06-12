

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  "${CMAKE_CURRENT_SOURCE_DIR}/controls"
  "${CMAKE_CURRENT_SOURCE_DIR}/visuals"
  "${CMAKE_CURRENT_SOURCE_DIR}/utility"
  )
# List all source files in the 'commands' folder
file(GLOB GUI_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
# add_library(GuiLib ${GUI_SOURCES})

# target_include_directories(GuiLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

