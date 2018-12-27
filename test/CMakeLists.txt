cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)

project (node-addon-api)

set(DISABLE_DEPRECATED_DEFAULT "false")


message("================================================================================")

# NODE_VERSION_*
execute_process(COMMAND node -p "process.version" OUTPUT_VARIABLE NODE_VERSION)
string(STRIP "${NODE_VERSION}" NODE_VERSION) # remove new line at end
STRING(REGEX MATCH "^v([^\.]+)\.([^\.]+)\.([^\.]+)$" NODE_VERSION "${NODE_VERSION}")
set(NODE_VERSION_MAJOR ${CMAKE_MATCH_1})
set(NODE_VERSION_MINOR ${CMAKE_MATCH_2})
set(NODE_VERSION_PATCH ${CMAKE_MATCH_3})
set(NODE_VERSION_SEMVER "${NODE_VERSION_MAJOR}.${NODE_VERSION_MINOR}.${NODE_VERSION_PATCH}")

message("NODE_VERSION                            = ${NODE_VERSION}")
message("NODE_VERSION_SEMVER                     = ${NODE_VERSION_SEMVER}")
message("NODE_VERSION_MAJOR                      = ${NODE_VERSION_MAJOR}")
message("NODE_VERSION_MINOR                      = ${NODE_VERSION_MINOR}")
message("NODE_VERSION_PATCH                      = ${NODE_VERSION_PATCH}")


# NAPI_VERSION
execute_process(COMMAND node -p "process.env['npm_config_NAPI_VERSION'] || ''" OUTPUT_VARIABLE NAPI_VERSION)
string(STRIP "${NAPI_VERSION}" NAPI_VERSION) # remove new line at end

message("NAPI_VERSION                            = ${NAPI_VERSION}")


# NODE_API_BUILTIN
set(NODE_API_BUILTIN "false")
if(("${NODE_VERSION_SEMVER}" VERSION_GREATER_EQUAL "9.0.0")
  OR ((${NODE_VERSION_MAJOR} EQUAL 8) AND ("${NODE_VERSION_SEMVER}" VERSION_GREATER_EQUAL "8.6.0"))
  OR ((${NODE_VERSION_MAJOR} EQUAL 6) AND ("${NODE_VERSION_SEMVER}" VERSION_GREATER_EQUAL "6.15.0"))
  OR ((${NODE_VERSION_MAJOR} EQUAL 6) AND ("${NODE_VERSION_SEMVER}" VERSION_GREATER_EQUAL "6.14.2"))
)
  set(NODE_API_BUILTIN "true")
else()
  set(NAPI_VERSION 1)
endif()

message("NODE_API_BUILTIN                        = ${NODE_API_BUILTIN}")


# disable_deprecated
execute_process(COMMAND node -p "process.env['npm_config_disable_deprecated'] || ''" OUTPUT_VARIABLE DISABLE_DEPRECATED)
string(STRIP "${DISABLE_DEPRECATED}" DISABLE_DEPRECATED) # remove new line at end

if(NOT "${DISABLE_DEPRECATED}" STREQUAL "true")
  set(DISABLE_DEPRECATED "${DISABLE_DEPRECATED_DEFAULT}")
endif()

message("disable_deprecated                      = ${DISABLE_DEPRECATED}")


# SOURCE_FILES
file(GLOB SOURCE_FILES "src/*.test.cc")
# SOURCE_FILES will be logged later


# NAPI_VERSION* compile options
if(NOT "${NAPI_VERSION}" STREQUAL "")
  set(DEFINITIONS_VERSION -DNAPI_VERSION=${NAPI_VERSION})
else()
  set(DEFINITIONS_VERSION -DNAPI_EXPERIMENTAL)
endif()

message("DEFINITIONS_VERSION                     = ${DEFINITIONS_VERSION}")
add_definitions("${DEFINITIONS_VERSION}")


# DISABLE_DEPRECATED compile options, source files
if ("${DISABLE_DEPRECATED}" STREQUAL "true")
  set(COMPILE_OPTIONS_DISABLE_DEPRECATED -DNODE_ADDON_API_DISABLE_DEPRECATED)
  add_compile_options("${COMPILE_OPTIONS_DISABLE_DEPRECATED}")
  message("COMPILE_OPTIONS_DISABLE_DEPRECATED      = ${COMPILE_OPTIONS_DISABLE_DEPRECATED}")
endif()


#env.json configure
set(NAPI_MODULES_FLAG "false")
if((NOT "${NODE_API_BUILTIN}" STREQUAL "true") AND ("${NODE_VERSION_MAJOR}" STREQUAL 8))
  set(NAPI_MODULES_FLAG "true")
endif()

configure_file("${CMAKE_SOURCE_DIR}/env.json.in" "${CMAKE_BINARY_DIR}/env.json")


# include directories
set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}" "${CMAKE_SOURCE_DIR}/.." "${CMAKE_JS_INC}")
include_directories("${INCLUDE_DIRECTORIES}")


# use builtin node api or compile shipped for old node versions
if("${NODE_API_BUILTIN}" STREQUAL "true")
  # set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}" "${CMAKE_SOURCE_DIR}/../external-napi")
else()
  add_subdirectory("${CMAKE_SOURCE_DIR}/../src" "${CMAKE_SOURCE_DIR}/build-node-api")
  include_directories("${INCLUDE_DIRECTORIES}") # need to update
endif()


string(REPLACE ";" "\n" SOURCE_FILES_LIST "${SOURCE_FILES}")
message("================================================================================")
message("INCLUDE_DIRECTORIES:")
message("${INCLUDE_DIRECTORIES}")
message("================================================================================")
message("SOURCE_FILES:")
message("${SOURCE_FILES_LIST}")
message("================================================================================")

# C/CXX flags
set(FLAGS "-Werror -Wall -Wextra -Wpedantic -Wunused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

message("CMAKE_C_FLAGS                           = ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_FLAGS                         = ${CMAKE_CXX_FLAGS}")
message("================================================================================")

# library binding
add_library(binding SHARED ${SOURCE_FILES})
target_compile_options(binding PRIVATE -DNAPI_CPP_EXCEPTIONS)
set_target_properties(binding PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(binding "${BINDING_LINK_LIBRARIES}")

# library binding_noexcept
add_library(binding_noexcept SHARED ${SOURCE_FILES})
target_compile_options(binding_noexcept PRIVATE -DNAPI_DISABLE_CPP_EXCEPTIONS)
set_target_properties(binding_noexcept PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(binding_noexcept "${BINDING_LINK_LIBRARIES}")