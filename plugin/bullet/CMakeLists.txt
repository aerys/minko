cmake_minimum_required(VERSION 3.5.1)

file (RELATIVE_PATH RELATIVE_LIBRARY_PATH ${MINKO_HOME}/../ ${CMAKE_CURRENT_SOURCE_DIR})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/enable.cmake DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${MINKO_HOME}/${RELATIVE_LIBRARY_PATH})

set (PROJECT_NAME "minko-plugin-bullet")

file (
    GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

file (
    GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src"
)

file (
    GLOB
    ${PROJECT_NAME}_EXCLUDE_ITEMS
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/MiniCL/MiniCL.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/Shared/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/Shared/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/SpuSampleTask/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet2/src/BulletMultiThreaded/SpuSampleTask/*.cpp"
)

foreach (${PROJECT_NAME}_EXCLUDE_ITEM ${${PROJECT_NAME}_EXCLUDE_ITEMS})
    list (
        REMOVE_ITEM
        ${PROJECT_NAME}_SRC
        ${${PROJECT_NAME}_EXCLUDE_ITEM}
    )
endforeach ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_serializer (${PROJECT_NAME})

if (UNIX)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -Wno-narrowing
        -Wno-int-to-pointer-cast
    )
endif ()

if (MSVC)
    target_compile_options (
        ${PROJECT_NAME} PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_SECURE_NO_DEPRECATE
    )
endif ()