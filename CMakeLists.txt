cmake_minimum_required(VERSION 3.15)
project(route_visualization LANGUAGES CXX)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

find_package(raylib 5.0 QUIET)

if(NOT raylib_FOUND)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_linux_amd64.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP
        TRUE)
  FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME} "src/main.cpp" "src/perlin.cpp" "src/map.cpp"
                               "src/algo.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
