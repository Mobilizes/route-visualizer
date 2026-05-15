cmake_minimum_required(VERSION 3.15)
project(route_visualization LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

if(EMSCRIPTEN)
  set(PLATFORM
      Web
      CACHE STRING "raylib target platform" FORCE)
  set(BUILD_EXAMPLES
      OFF
      CACHE BOOL "Build raylib examples" FORCE)
  set(BUILD_GAMES
      OFF
      CACHE BOOL "Build raylib games" FORCE)
else()
  find_package(raylib 5.5 QUIET)
endif()

if(NOT raylib_FOUND)
  FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5)
  FetchContent_MakeAvailable(raylib)
endif()

add_executable(${PROJECT_NAME} "src/main.cpp" "src/perlin.cpp" "src/map.cpp"
                               "src/algo.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(EMSCRIPTEN)
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
  target_link_options(${PROJECT_NAME} PRIVATE "-sUSE_GLFW=3" "-sALLOW_MEMORY_GROWTH=1")
endif()

if(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
