cmake_minimum_required(VERSION 3.15)

project(precompiler LANGUAGES CXX)

include(../../cmake/folders.cmake)

add_executable(precompiler source/main.cpp)
target_compile_features(precompiler PRIVATE cxx_std_17)

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(GNUInstallDirs)

  install(TARGETS precompiler COMPONENT precompiler)
endif()

add_folders(Precompiler)
