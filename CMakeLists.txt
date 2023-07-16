cmake_minimum_required(VERSION 3.15)

include(cmake/prelude.cmake)

project(
    build-tool
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/precompiler.cmake)

# ---- Declare library ----

add_library(
    build-tool_lib OBJECT
    source/lib.cpp
)

target_include_directories(
    build-tool_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

prec_target_sources(build-tool_lib PRIVATE source/lib.prec)
prec_target_include_directories(build-tool_lib PRIVATE source)

target_compile_features(build-tool_lib PUBLIC cxx_std_17)

# ---- Declare executable ----

add_executable(build-tool_exe source/main.cpp)
add_executable(build-tool::exe ALIAS build-tool_exe)

set_property(TARGET build-tool_exe PROPERTY OUTPUT_NAME build-tool)

target_compile_features(build-tool_exe PRIVATE cxx_std_17)

target_link_libraries(build-tool_exe PRIVATE build-tool_lib)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT build-tool_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of build-tool"
  )
endif()

include(cmake/dev-mode.cmake)
