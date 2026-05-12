cmake_minimum_required(VERSION 3.15)
project(route_visualization)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

find_package(raylib 3.0 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME} "src/main.cpp" "src/perlin.cpp" "src/map.cpp"
                               "src/algo.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} raylib)

if(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework IOKit")
  target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
  target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
