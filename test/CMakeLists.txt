# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(build-toolTests LANGUAGES CXX)

# ---- Tests ----

add_executable(build-tool_test source/build-tool_test.cpp)
target_link_libraries(build-tool_test PRIVATE build-tool_lib)
target_compile_features(build-tool_test PRIVATE cxx_std_17)

add_test(NAME build-tool_test COMMAND build-tool_test)

# ---- End-of-file commands ----

add_folders(Test)
