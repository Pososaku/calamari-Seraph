cmake_minimum_required(VERSION 3.10)

project(calamari VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMMON_DIR ${CMAKE_SOURCE_DIR}/src/common)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/src/imgui)
set(REACTOR_DIR ${CMAKE_SOURCE_DIR}/src/calamari.reactor)
set(PUBLIC_DIR ${CMAKE_SOURCE_DIR}/src/calamari.public)
set(XRAY_DIR ${CMAKE_SOURCE_DIR}/src/plugin.xray)
set(AIMBOT_DIR ${CMAKE_SOURCE_DIR}/src/plugin.aimbot)
set(ASSIST_DIR ${CMAKE_SOURCE_DIR}/src/plugin.assist)
set(RADAR_DIR ${CMAKE_SOURCE_DIR}/src/plugin.radar)
set(WSOCKLOADER_DIR ${CMAKE_SOURCE_DIR}/src/wsockloader)

file(GLOB IMGUI_FILES
    "${IMGUI_DIR}/*.h"
    "${IMGUI_DIR}/*.cpp"
)

add_library(calamari SHARED 
    ${IMGUI_FILES}
    ${COMMON_DIR}/Pocket.cpp ${COMMON_DIR}/Pocket.h
    ${COMMON_DIR}/util.cpp ${COMMON_DIR}/util.h
    ${REACTOR_DIR}/dllmain.cpp
    ${REACTOR_DIR}/kiero/kiero.cpp ${REACTOR_DIR}/kiero/kiero.h
    ${REACTOR_DIR}/reactor.cpp ${REACTOR_DIR}/reactor.h
    ${REACTOR_DIR}/hooks/hooks.h
    ${REACTOR_DIR}/hooks/drm.cpp
    ${REACTOR_DIR}/hooks/dx.cpp
    ${REACTOR_DIR}/hooks/byond.cpp
    ${REACTOR_DIR}/hooks/cid.cpp
    ${PUBLIC_DIR}/byond.cpp ${PUBLIC_DIR}/byond.h
    ${PUBLIC_DIR}/events.h
    ${PUBLIC_DIR}/plugin.h
)
target_compile_options(calamari PRIVATE /Qspectre- /Ob1)
target_link_libraries(calamari ${CMAKE_SOURCE_DIR}/libs/VMProtectSDK32.lib shlwapi.lib comsuppw.lib)
target_link_libraries(calamari debug ${CMAKE_SOURCE_DIR}/libs/minhook-debug.lib)
target_link_libraries(calamari optimized ${CMAKE_SOURCE_DIR}/libs/minhook.lib)
target_compile_definitions(calamari PRIVATE WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS)

add_library(calamari-xray SHARED
    ${XRAY_DIR}/Pocket.cpp ${COMMON_DIR}/Pocket.h
    ${XRAY_DIR}/xray.cpp
)
target_link_libraries(calamari-xray calamari)
target_compile_definitions(calamari-xray PRIVATE WIN32_LEAN_AND_MEAN)

add_library(calamari-aimbot SHARED
    ${AIMBOT_DIR}/aimbotipc.h
    ${AIMBOT_DIR}/aimbot.h
    ${AIMBOT_DIR}/aimbot.cpp
)
target_link_libraries(calamari-aimbot calamari)

add_library(calamari-assist SHARED
    ${ASSIST_DIR}/assist.cpp
)
target_link_libraries(calamari-assist calamari)

add_library(calamari-radar SHARED
    ${RADAR_DIR}/radar.h
    ${RADAR_DIR}/radar.cpp
)
target_link_libraries(calamari-radar calamari)
