cmake_minimum_required(VERSION 3.25)

project(VoxelEvaEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.3
)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw spdlog glm)

function(ve_set_folder_if_target target_name folder_name)
    if(TARGET ${target_name})
        set_target_properties(${target_name} PROPERTIES FOLDER ${folder_name})
    endif()
endfunction()

ve_set_folder_if_target(glfw "Dependencies")
ve_set_folder_if_target(update_mappings "Dependencies")
ve_set_folder_if_target(spdlog "Dependencies")
ve_set_folder_if_target(glm "Dependencies")

function(ve_set_common_settings target_name)
    target_compile_features(${target_name} PUBLIC cxx_std_20)

    target_include_directories(${target_name}
        PUBLIC
            ${CMAKE_SOURCE_DIR}/Engine/src
    )

    target_compile_definitions(${target_name}
        PRIVATE
            $<$<CONFIG:Debug>:VE_DEBUG>
            $<$<CONFIG:Release>:VE_RELEASE>
            $<$<CONFIG:RelWithDebInfo>:VE_RELEASE>
            $<$<CONFIG:MinSizeRel>:VE_DIST>
    )

    if(WIN32)
        target_compile_definitions(${target_name} PRIVATE VE_PLATFORM_WINDOWS)
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${VE_OUTPUT_DIR}/$<CONFIG>/${target_name}"
        LIBRARY_OUTPUT_DIRECTORY "${VE_OUTPUT_DIR}/$<CONFIG>/${target_name}"
        ARCHIVE_OUTPUT_DIRECTORY "${VE_OUTPUT_DIR}/$<CONFIG>/${target_name}"
    )
endfunction()

function(ve_source_group target_root source_files)
    source_group(TREE ${CMAKE_SOURCE_DIR}/${target_root}/src PREFIX "Source" FILES ${${source_files}})
endfunction()

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    Engine/src/*.h
    Engine/src/*.hpp
    Engine/src/*.cpp
)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    Game/src/*.h
    Game/src/*.hpp
    Game/src/*.cpp
)

file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS
    Editor/src/*.h
    Editor/src/*.hpp
    Editor/src/*.cpp
)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    Server/src/*.h
    Server/src/*.hpp
    Server/src/*.cpp
)

add_library(Engine STATIC
    ${ENGINE_SOURCES}
)

ve_set_common_settings(Engine)
ve_source_group("Engine" ENGINE_SOURCES)

target_link_libraries(Engine
    PUBLIC
        glfw
        spdlog::spdlog
         glm::glm
)

target_compile_definitions(Engine
    PRIVATE
        GLFW_INCLUDE_NONE
)

set_target_properties(Engine PROPERTIES FOLDER "VoxelEvaEngine")

add_executable(Game
    ${GAME_SOURCES}
)

ve_set_common_settings(Game)
ve_source_group("Game" GAME_SOURCES)

target_include_directories(Game
    PRIVATE
        Game/src
)

target_link_libraries(Game
    PRIVATE
        Engine
)

set_target_properties(Game PROPERTIES FOLDER "VoxelEvaEngine")

add_executable(Editor
    ${EDITOR_SOURCES}
)

ve_set_common_settings(Editor)
ve_source_group("Editor" EDITOR_SOURCES)

target_include_directories(Editor
    PRIVATE
        Editor/src
)

target_link_libraries(Editor
    PRIVATE
        Engine
)

set_target_properties(Editor PROPERTIES FOLDER "VoxelEvaEngine")

add_executable(Server
    ${SERVER_SOURCES}
)

ve_set_common_settings(Server)
ve_source_group("Server" SERVER_SOURCES)

target_include_directories(Server
    PRIVATE
        Server/src
)

target_link_libraries(Server
    PRIVATE
        Engine
)

set_target_properties(Server PROPERTIES FOLDER "VoxelEvaEngine")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)