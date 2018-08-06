cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-fx")

file (GLOB
    ${PROJECT_NAME}_SRC
    "src/minko/material/WaterMaterial.cpp"
    "include/minko/material/WaterMaterial.hpp"
    "include/minko/MinkoFX.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "src"
    "include"
)

minko_add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_INCLUDE})