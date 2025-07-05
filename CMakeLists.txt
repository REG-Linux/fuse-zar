cmake_minimum_required(VERSION 3.16)
project(fuse_zar VERSION 0.1 LANGUAGES CXX)
enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto=auto")

# Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED fuse3)

# Include zarchive
add_subdirectory(zarchive)

# Include dirs
include_directories(${FUSE3_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/zarchive/include)

# Source files
set(SOURCES
    zarchive_fuse.c
    zarchive_wrapper.cpp
    zarchive/src/sha_256.c
    zarchive/src/zarchivereader.cpp
)

add_executable(fuse-zar ${SOURCES})

target_link_libraries(fuse-zar
    ${FUSE3_LIBRARIES}
    zstd
)
