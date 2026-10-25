cmake_minimum_required(VERSION 3.17)
project(CossackQuest)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd

# Adding Raylib: use a system install (e.g. Nix) if present, otherwise fetch it
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
    FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
    FetchContent_MakeAvailable(raylib)
endif ()

# Adding our source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "../${CMAKE_CURRENT_LIST_DIR}/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "../${CMAKE_CURRENT_LIST_DIR}/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

add_executable(CossackQuest ../main.cpp ../Game.cpp ../Game.hpp ../PlayerControl.cpp ../PlayerControl.hpp
        ../shaders/rlights.h ../GameObject.hpp ../Player.hpp ../Weapon.hpp ../GameObject.cpp ../Enemy.hpp ../Enemy.cpp
        ../util.h ../Ui.hpp ../Ui.cpp ../NavGrid.hpp ../NavGrid.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)