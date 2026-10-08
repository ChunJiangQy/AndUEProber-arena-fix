cmake_minimum_required(VERSION 3.22.1)

if(NOT CMAKE_TOOLCHAIN_FILE)
    if(NOT DEFINED ENV{NDK_HOME})
        message(FATAL_ERROR "Set NDK_HOME to the Android NDK, or pass CMAKE_TOOLCHAIN_FILE.")
    endif()
    file(TO_CMAKE_PATH "$ENV{NDK_HOME}" NDK_HOME_CMAKE)
    set(CMAKE_TOOLCHAIN_FILE "${NDK_HOME_CMAKE}/build/cmake/android.toolchain.cmake")
endif()
set(ANDROID_ABI arm64-v8a CACHE STRING "")
set(ANDROID_PLATFORM 27 CACHE STRING "")

project(AndUEProber LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(DUMPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/AndUEDumper)
set(DUMPER_SRC ${DUMPER_DIR}/AndUEDumper/src)
set(DUMPER_DEPS ${DUMPER_DIR}/deps)
set(KITTY_DIR ${DUMPER_DEPS}/KittyMemoryEx/KittyMemoryEx)

if(NOT EXISTS ${KITTY_DIR}/KittyMemoryMgr.cpp)
    message(FATAL_ERROR "Run: git submodule update --init --recursive")
endif()

file(GLOB KITTY_SOURCES CONFIGURE_DEPENDS ${KITTY_DIR}/*.cpp)
file(GLOB UE_SOURCES CONFIGURE_DEPENDS ${DUMPER_SRC}/UE/*.cpp)
set(SOURCES
    ${KITTY_SOURCES}
    ${UE_SOURCES}
    ${DUMPER_SRC}/Dumper.cpp
    ${DUMPER_SRC}/UPackageGenerator.cpp
    ${DUMPER_SRC}/Utils/BufferFmt.cpp
    ${DUMPER_DEPS}/fmt/format.cc
    source/Dump.cpp
    source/Prober.cpp
)

# The dumper's logger writes to logcat in the library and to stdout in the
# executable, so each target compiles the sources with its own definitions.
add_library(AndUEProber SHARED ${SOURCES} source/Library.cpp)
# Target names must differ beyond case for case-insensitive build directories.
add_executable(AndUEProberExecutable ${SOURCES} source/Executable.cpp)
set_target_properties(AndUEProberExecutable PROPERTIES OUTPUT_NAME andueprober)
target_compile_definitions(AndUEProberExecutable PRIVATE kEXECUTABLE)

foreach(target AndUEProber AndUEProberExecutable)
    target_include_directories(${target} PRIVATE source ${DUMPER_SRC} ${DUMPER_DEPS} ${KITTY_DIR})
    target_compile_definitions(${target} PRIVATE kNO_KEYSTONE $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror -fexceptions)
    target_link_libraries(${target} PRIVATE log)
    target_link_options(${target} PRIVATE $<$<CONFIG:Release>:-s>)
endforeach()
