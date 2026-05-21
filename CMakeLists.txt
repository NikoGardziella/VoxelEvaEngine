cmake_minimum_required(VERSION 3.25)

project(VoxelEvaEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

option(VE_BUILD_EDITOR "Build the editor application" ON)
option(VE_BUILD_GAME "Build the game application" ON)
option(VE_BUILD_SERVER "Build the server application" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(VE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4)
FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git GIT_TAG v1.15.3)
FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw spdlog glm imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui
    PUBLIC
        glfw
        Vulkan::Vulkan
)

set_target_properties(imgui PROPERTIES FOLDER "Dependencies")

find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

find_program(GLSLC glslc HINTS "$ENV{VULKAN_SDK}/Bin")

if(NOT GLSLC)
    message(FATAL_ERROR "glslc not found. Install Vulkan SDK or add glslc to PATH.")
endif()

function(ve_set_folder_if_target target_name folder_name)
    if(TARGET ${target_name})
        set_target_properties(${target_name} PROPERTIES FOLDER ${folder_name})
    endif()
endfunction()

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

ve_set_folder_if_target(glfw "Dependencies")
ve_set_folder_if_target(update_mappings "Dependencies")
ve_set_folder_if_target(spdlog "Dependencies")
ve_set_folder_if_target(glm "Dependencies")

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    Engine/src/*.h
    Engine/src/*.hpp
    Engine/src/*.cpp
)

file(GLOB_RECURSE ENGINE_SHADER_SOURCE_FILES CONFIGURE_DEPENDS
    Engine/assets/shaders/*.vert
    Engine/assets/shaders/*.frag
    Engine/assets/shaders/*.comp
    Engine/assets/shaders/*.glsl
)

set(ENGINE_SHADER_SPV_FILES)

foreach(shader_file ${ENGINE_SHADER_SOURCE_FILES})
    get_filename_component(shader_ext ${shader_file} EXT)

    if(shader_ext STREQUAL ".vert" OR shader_ext STREQUAL ".frag" OR shader_ext STREQUAL ".comp")
        set(spv_file "${shader_file}.spv")

        add_custom_command(
            OUTPUT ${spv_file}
            COMMAND ${GLSLC} ${shader_file} -o ${spv_file}
            DEPENDS ${shader_file}
            COMMENT "Compiling shader ${shader_file}"
            VERBATIM
        )

        list(APPEND ENGINE_SHADER_SPV_FILES ${spv_file})
    endif()
endforeach()

add_custom_target(CompileEngineShaders DEPENDS ${ENGINE_SHADER_SPV_FILES})

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
    ${ENGINE_SHADER_SOURCE_FILES}
    ${ENGINE_SHADER_SPV_FILES}
)

add_dependencies(Engine CompileEngineShaders)

ve_set_common_settings(Engine)
ve_source_group("Engine" ENGINE_SOURCES)

source_group("Shaders" FILES ${ENGINE_SHADER_SOURCE_FILES} ${ENGINE_SHADER_SPV_FILES})

set_source_files_properties(${ENGINE_SHADER_SOURCE_FILES} ${ENGINE_SHADER_SPV_FILES} PROPERTIES
    VS_TOOL_OVERRIDE "None"
)

target_link_libraries(Engine
    PUBLIC
        glfw
        spdlog::spdlog
        glm::glm
        Vulkan::Vulkan
        Vulkan::shaderc_combined
)

target_compile_definitions(Engine
    PRIVATE
        GLFW_INCLUDE_NONE
        VE_ENGINE_ASSET_DIR="${CMAKE_SOURCE_DIR}/Engine/assets"
        VE_SHADER_SOURCE_DIR="${CMAKE_SOURCE_DIR}/Engine/assets/shaders/src"
        VE_SHADER_CACHE_DIR="${CMAKE_SOURCE_DIR}/Engine/assets/shaders/cache/vulkan"
)

set_target_properties(Engine PROPERTIES FOLDER "VoxelEvaEngine")

if(VE_BUILD_GAME)
    add_executable(Game ${GAME_SOURCES})
    ve_set_common_settings(Game)
    ve_source_group("Game" GAME_SOURCES)
    target_include_directories(Game PRIVATE Game/src)
    target_link_libraries(Game PRIVATE Engine)
    set_target_properties(Game PROPERTIES FOLDER "VoxelEvaEngine")
endif()

if(VE_BUILD_EDITOR)
    add_executable(Editor ${EDITOR_SOURCES})
    ve_set_common_settings(Editor)
    ve_source_group("Editor" EDITOR_SOURCES)
    target_include_directories(Editor PRIVATE Editor/src)
    target_link_libraries(Editor PRIVATE Engine imgui)
    set_target_properties(Editor PROPERTIES FOLDER "VoxelEvaEngine")
endif()

add_executable(Server ${SERVER_SOURCES})
ve_set_common_settings(Server)
ve_source_group("Server" SERVER_SOURCES)

target_include_directories(Server PRIVATE Server/src)

target_link_libraries(Server PRIVATE Engine)

set_target_properties(Server PROPERTIES
    FOLDER "VoxelEvaEngine"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

if(VE_BUILD_EDITOR)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)
elseif(VE_BUILD_GAME)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Game)
endif()
