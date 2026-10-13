cmake_minimum_required(VERSION 3.28)
set (TARGET_NAME xmplay)
project (${TARGET_NAME})

add_definitions(-DUSE_SDL)

add_compile_options(-Wfatal-errors)
add_compile_options(-Werror=return-type)
add_compile_options(-fmax-errors=1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${TARGET_NAME} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED 20)

find_package(SDL2 REQUIRED)
find_package(libxmp REQUIRED)

target_link_libraries(${TARGET_NAME} SDL2::SDL2 libxmp::xmp_shared)

