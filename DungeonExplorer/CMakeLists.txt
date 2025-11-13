cmake_minimum_required(VERSION 3.16)
project(DungeonExplorer)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML (version 3.x installed)
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

# Find TGUI - Disabled, not compatible with SFML 3.x build
# find_package(TGUI 1 REQUIRED)

# Include directories
include_directories(include)
include_directories(include/DataStructures)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Player.cpp
    src/Dungeon.cpp
    src/Enemy.cpp
    src/SkillTree.cpp
    src/UIManager.cpp
    src/Renderer.cpp
    src/DSAVisualizer.cpp
    src/AssetManager.cpp
    src/DungeonLevelManager.cpp
    src/ItemManager.cpp
    src/Shop.cpp
    src/DataStructures/Stack.cpp
    src/DataStructures/Queue.cpp
    src/DataStructures/LinkedList.cpp
    src/DataStructures/Heap.cpp
    src/DataStructures/Tree.cpp
    src/DataStructures/Graph.cpp
    src/DataStructures/HashTable.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Copy assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# Platform-specific settings
if(WIN32)
    # Copy SFML DLLs on Windows
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(SFML_DLL_SUFFIX "-d-2")
    else()
        set(SFML_DLL_SUFFIX "-2")
    endif()
endif()
