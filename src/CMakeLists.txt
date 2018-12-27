cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC flag

# TODO: "${CMAKE_SOURCE_DIR}/../src" is unclean
set(INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}" "${CMAKE_SOURCE_DIR}/../src" PARENT_SCOPE)

if(NOT "${NAPI_VERSION}" STREQUAL "")
  add_definitions(-DNAPI_VERSION=${NAPI_VERSION})
endif()

add_library(node-api STATIC node_api.cc node_internals.cc)
target_compile_options(node-api PRIVATE -DEXTERNAL_NAPI)
set(BINDING_LINK_LIBRARIES "${BINDING_LINK_LIBRARIES}" node-api PARENT_SCOPE)