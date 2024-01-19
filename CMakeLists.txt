cmake_minimum_required(VERSION 3.5)
project(RTCPU_PGE)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP true)
FetchContent_MakeAvailable(json)

add_compile_options()
add_link_options()

add_subdirectory(src/olc)
add_subdirectory(src/threadpool)
set(sources
    src/camera.cpp
    src/hittablelist.cpp
    src/main.cpp
    src/mainscreen.cpp
    src/material.cpp
    src/sphere.cpp
    src/util.cpp
    src/vec3.cpp
    src/interval.cpp
    src/aabb.cpp
    src/bvh.cpp
    src/triangle.cpp
    src/mat3.cpp
    src/sceneloading.cpp
)


add_custom_target(scenes ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/scenes
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/scenes ${CMAKE_CURRENT_BINARY_DIR}/scenes
    DEPENDS ${MY_TARGET}
)

add_custom_target(models ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/models
    DEPENDS ${MY_TARGET}
)

add_custom_target(config ALL
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/rtconfig.json
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/rtconfig.json ${CMAKE_CURRENT_BINARY_DIR}/rtconfig.json
    DEPENDS ${MY_TARGET}
)

add_executable(rtcpu ${sources})
target_link_libraries(rtcpu PRIVATE olcpge)
target_link_libraries(rtcpu PRIVATE threadpool)
target_link_libraries(rtcpu PRIVATE -lX11 -lGL -lpthread -lpng -pg)

target_link_libraries(rtcpu PRIVATE nlohmann_json::nlohmann_json)





