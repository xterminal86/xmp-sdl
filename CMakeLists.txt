cmake_minimum_required(VERSION 3.17)
set (TARGET_NAME xmplay-sdl)
project (${TARGET_NAME})

add_definitions(-DUSE_SDL)

add_compile_options(-Wfatal-errors)
add_compile_options(-Werror=return-type)
add_compile_options(-fmax-errors=1)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  add_definitions(-DRELEASE_BUILD)
else()
  add_definitions(-DDEBUG_BUILD)
endif()

file(
  GLOB
  SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/extra/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/extra/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/app/*.h
)

add_executable(${TARGET_NAME} ${SOURCES})

set_target_properties(
  ${TARGET_NAME}
  PROPERTIES
  CMAKE_CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 20
)

find_package(SDL2 REQUIRED)

find_package(libxmp REQUIRED)

target_include_directories(
  ${TARGET_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/app
  ${CMAKE_CURRENT_SOURCE_DIR}/extra
)

target_link_libraries(${TARGET_NAME} SDL2::SDL2 libxmp::xmp_shared)

add_subdirectory(tests)
