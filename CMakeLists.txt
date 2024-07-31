cmake_minimum_required(VERSION 3.21)
project(qlang CXX)

message("${PROJECT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  add_subdirectory(tests)
endif()

install(DIRECTORY include/ DESTINATION include) # HACK: use target_sources()?
set(INSTALL_GTEST OFF)
include(CPack)
