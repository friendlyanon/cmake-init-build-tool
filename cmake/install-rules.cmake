install(
    TARGETS build-tool_exe
    RUNTIME COMPONENT build-tool_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
