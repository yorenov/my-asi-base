cmake_minimum_required(VERSION 3.2)
project(asi-base)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    CACHE PATH "Exec")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    CACHE PATH "Libs")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  
set(CMAKE_BUILD_TYPE "Debug")  
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018")

include(FetchContent)

FetchContent_Declare(sampapi GIT_REPOSITORY https://github.com/BlastHackNet/SAMP-API.git GIT_TAG f89b35edb7eaf5db374e32a63f2366c4bb714418)
FetchContent_Declare(ktsignal GIT_REPOSITORY https://github.com/kin4stat/ktsignal.git GIT_TAG f08a19d9a00ad5284871bf44178d835da7b26ebf)
FetchContent_Declare(kthook GIT_REPOSITORY https://github.com/kin4stat/kthook.git GIT_TAG e2817793c32570f95fe71208dc64af609c653abe)
FetchContent_Declare(RakHook GIT_REPOSITORY https://github.com/imring/RakHook.git GIT_TAG 625fd17897395997feb56ae293c7c07dbe49d26b)
FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG f50ddc431e3b8840036e88abc4c3cf74500aa12b)

FetchContent_MakeAvailable(sampapi ktsignal kthook rakhook imgui)

set(IMGUI_DIR "${CMAKE_BINARY_DIR}/_deps/imgui-src")

FetchContent_GetProperties(imgui)

if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    add_subdirectory(${imgui_SOURCE_DIR} ${imgui_BINARY_DIR})
endif()

#include_directories($ENV{PLUGIN_SDK_DIR}/plugin_sa)
#include_directories($ENV{PLUGIN_SDK_DIR}/plugin_sa/game_sa)
#include_directories($ENV{PLUGIN_SDK_DIR}/shared)
#include_directories($ENV{PLUGIN_SDK_DIR}/shared/game)
include_directories(${IMGUI_DIR})
include_directories(${IMGUI_DIR}/backends)
add_library(${PROJECT_NAME} SHARED
        src/fish_bot/fishbot.cpp
        src/fish_bot/fishbot.h)

target_link_libraries(${PROJECT_NAME} PUBLIC sampapi kthook rakhook)

target_sources(${PROJECT_NAME} PRIVATE
        src/ImGui-set/menu.cpp
        src/ImGui-set/ImGui_set.cpp
        src/recorder/recorder.cpp
        src/utils/Utils.cpp
        src/rakhookT.cpp
        src/main.cpp
   ${imgui_SOURCE_DIR}/imgui.cpp
   ${imgui_SOURCE_DIR}/imgui_draw.cpp
   ${imgui_SOURCE_DIR}/imgui_demo.cpp
   ${imgui_SOURCE_DIR}/imgui_tables.cpp
   ${imgui_SOURCE_DIR}/imgui_widgets.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_dx9.cpp
   ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
 )

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        SUFFIX ".asi"
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
