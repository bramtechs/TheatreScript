cmake_minimum_required(VERSION 3.6)
project(theatrescript CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory("libs/magic_enum")

# Add unit test framework
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cc" "${CMAKE_CURRENT_LIST_DIR}/src/*.hh" "${CMAKE_CURRENT_LIST_DIR}/include/*.hh"
    "${CMAKE_CURRENT_LIST_DIR}/tests/*.cc" "${CMAKE_CURRENT_LIST_DIR}/tests/*.hh") # Define PROJECT_SOURCES as a list of all source files

# Declaring our executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/")
target_link_libraries(${PROJECT_NAME} PRIVATE
    magic_enum
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})