# Exacute .bat and read the values
execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/get_processors.bat
    COMMAND_ARGS
    OUTPUT_VARIABLE OUTPUT_VAR
    RESULT_VARIABLE RESULT_VAR
)

if(RESULT_VAR EQUAL 0)
    string(REGEX REPLACE "\n" ";" OUTPUT_LIST "${OUTPUT_VAR}")
    list(GET OUTPUT_LIST 0 MACHINE_NUM_PROCESSORS)
    list(GET OUTPUT_LIST 1 MACHINE_NUM_CORES)
else()
    message(FATAL_ERROR "Error running get_processors.bat: ${RESULT_VAR}")
endif()


# Take config file template (config.hpp.in) and copy it into build directory
# Add the created file to include path
set(CONFIG_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/configuration/include")
configure_file(
    "Config.hpp.in"
    "${CONFIG_INCLUDE_DIRECTORY}/Config.hpp" ESCAPE_QUOTES
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CONFIG_INCLUDE_DIRECTORY})
