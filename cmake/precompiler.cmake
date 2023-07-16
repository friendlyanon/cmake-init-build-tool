cmake_minimum_required(VERSION 3.15)

get_cmake_property(role CMAKE_ROLE)
if(role STREQUAL "SCRIPT")
  execute_process(
      COMMAND ${COMMAND}
      RESULT_VARIABLE result
      OUTPUT_VARIABLE output
      ERROR_VARIABLE output
  )

  if(NOT result EQUAL "0")
    message(FATAL_ERROR "Failed with ${result}\nCommand: ${COMMAND}\n${output}")
  endif()

  return()
endif()

include_guard(GLOBAL)

set(PRECOMPILER_EXECUTABLE "" CACHE FILEPATH "Path to the precompiler executable")
if(PRECOMPILER_EXECUTABLE)
  set_property(GLOBAL PROPERTY PREC_COMPILER "${PRECOMPILER_EXECUTABLE}")
  set_property(GLOBAL PROPERTY PREC_DEPENDS "")
else()
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tools/precompiler" tools/precompiler EXCLUDE_FROM_ALL)
  set_property(GLOBAL PROPERTY PREC_COMPILER "\$<TARGET_FILE:precompiler>")
  set_property(GLOBAL PROPERTY PREC_DEPENDS ";precompiler")
endif()

set_property(GLOBAL PROPERTY PREC_SILENT_SCRIPT "${CMAKE_CURRENT_LIST_FILE}")

function(prec_target_sources target)
  set(multi PRIVATE PUBLIC INTERFACE)
  cmake_parse_arguments(PARSE_ARGV 1 SRC "" "" "${multi}")

  if(SRC_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments: \"${SRC_UNPARSED_ARGUMENTS}\"")
  endif()

  get_cmake_property(depends PREC_DEPENDS)
  get_cmake_property(compiler PREC_COMPILER)
  get_cmake_property(self PREC_SILENT_SCRIPT)
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/prec")
  foreach(category IN LISTS multi)
    foreach(file IN LISTS "SRC_${category}")
      get_filename_component(input "${file}" REALPATH)
      get_filename_component(filename "${file}" NAME)
      set(output "${CMAKE_CURRENT_BINARY_DIR}/prec/${filename}.cpp")
      file(RELATIVE_PATH relative "${CMAKE_CURRENT_BINARY_DIR}" "${output}")

      set(delim "\$<SEMICOLON>include=")
      set(includes "\$<TARGET_PROPERTY:${target},PREC_INCLUDE_DIRECTORIES>")
      set(includes "\$<REMOVE_DUPLICATES:${includes}>")
      set(includes "\$<FILTER:${includes},EXCLUDE,^\$>")
      set(includes "\$<IF:\$<STREQUAL:${includes},>,,${delim}\$<JOIN:${includes},${delim}>>")

      add_custom_command(
          OUTPUT "${output}"
          COMMAND "${CMAKE_COMMAND}"
          "-DCOMMAND=${compiler};input=${input};output=${output}${includes}"
          -P "${self}"
          MAIN_DEPENDENCY "${input}"
          DEPENDS "${self}${depends}"
          WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
          COMMENT "Precompiling ${relative}"
          VERBATIM
      )
      target_sources("${target}" "${category}" "${output}")
    endforeach()
  endforeach()
endfunction()

function(prec_target_include_directories target)
  cmake_parse_arguments(PARSE_ARGV 1 INC "" "" PRIVATE)

  if(INC_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown arguments: \"${INC_UNPARSED_ARGUMENTS}\"")
  endif()

  foreach(dir IN LISTS INC_PRIVATE)
    get_filename_component(dir "${dir}" REALPATH)
    set_property(TARGET "${target}" APPEND PROPERTY PREC_INCLUDE_DIRECTORIES "${dir}")
  endforeach()
endfunction()
