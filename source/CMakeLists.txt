cmake_minimum_required(VERSION 3.0.0)
project(fem VERSION 0.1.0)

include(CTest)
enable_testing()

add_compile_options(-mwindows)

SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "C:/Boost/include/boost-1_81")
SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Boost/lib")

add_executable(fem WIN32 main.cpp)

add_subdirectory(dependencies/imgui)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glad)

target_link_libraries(fem glfw opengl32 imgui glad)

target_include_directories(fem PRIVATE dependencies/imgui)
target_include_directories(fem PRIVATE dependencies/imgui/backends)
target_include_directories(fem PRIVATE dependencies/glfw/include)
target_include_directories(fem PRIVATE dependencies/glad/include)
target_include_directories(fem PRIVATE dependencies/eigen)


find_package(Boost)

if(Boost_FOUND)
    target_link_libraries(fem Boost::boost)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
