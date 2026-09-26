cmake_minimum_required(VERSION 3.16)
project(TowerDefense LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Put the .exe in build/bin so it's easy to find on every generator.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Static SFML = no DLLs to copy next to the exe on Windows.
set(BUILD_SHARED_LIBS OFF)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SFML)

add_executable(towerdefense
    src/main.cpp
    src/Map/Map.cpp
    src/Map/MapGenerator.cpp
    src/GUI/MapView.cpp
    src/GUI/TowerView.cpp
    src/GUI/Hud.cpp
    src/GUI/SimpleText.cpp
    src/Critter/Critter.cpp
    src/Tower/Tower.cpp
    src/Tower/DirectDamageTower.cpp
    src/Tower/AreaDamageTower.cpp
    src/Tower/SlowingTower.cpp
)

target_include_directories(towerdefense PRIVATE src)
target_link_libraries(towerdefense PRIVATE sfml-graphics)

if(MSVC)
    target_compile_options(towerdefense PRIVATE /W4)
else()
    target_compile_options(towerdefense PRIVATE -Wall -Wextra)
endif()
