include(cmake/folders.cmake)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

add_custom_target(
    run-exe
    COMMAND build-tool_exe
    VERBATIM
)
add_dependencies(run-exe build-tool_exe)

add_folders(Project)
