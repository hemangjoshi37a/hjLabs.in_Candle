# CAD/CAM shared core library for hjLabs.in Candle.
# Builds as a static library linked into the main `candle` target, and can also
# be configured standalone for fast unit-test iteration:
#   /usr/bin/cmake -S src/cadcam -B build-cadcam && cmake --build build-cadcam
#   ctest --test-dir build-cadcam --output-on-failure

cmake_minimum_required(VERSION 3.10)

set(CADCAM_STANDALONE OFF)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(cadcam VERSION 0.1 LANGUAGES CXX)
    set(CADCAM_STANDALONE ON)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui REQUIRED)

set(CADCAM_SOURCES
    geometry.cpp
    entity.cpp
    toolpath.cpp
    gcodeemitter.cpp
    dxfimporter.cpp
    offset.cpp
    camoperations.cpp
    soldering.cpp
    strokefont.cpp
    gerberimporter.cpp
    excellonimporter.cpp
    pcbcam.cpp
)

add_library(cadcam STATIC ${CADCAM_SOURCES})
target_include_directories(cadcam PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cadcam PUBLIC Qt5::Core Qt5::Gui)

option(BUILD_CADCAM_TESTS "Build CAD/CAM core unit tests" ${CADCAM_STANDALONE})
if(BUILD_CADCAM_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
