cmake_minimum_required(VERSION 3.5.1)

set (PROJECT_NAME "minko-plugin-websocket")

file (GLOB_RECURSE
    ${PROJECT_NAME}_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file (GLOB
    ${PROJECT_NAME}_INCLUDE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# start excludes
if (EMSCRIPTEN)
    list (APPEND
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/net/NativeWebSocketImpl.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/net/NativeWebSocketImpl.cpp"
    )
    # might need to add 'files'
endif ()

if (NOT EMSCRIPTEN)
    list (APPEND
        ${PROJECT_NAME}_EXCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/net/EmscriptenWebSocketImpl.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/minko/net/EmscriptenWebSocketImpl.cpp"
    )
endif ()
# end excludes

foreach (OBJ ${${PROJECT_NAME}_EXCLUDES})
    list(REMOVE_ITEM ${PROJECT_NAME}_SRC ${OBJ})
endforeach ()

minko_add_library (${PROJECT_NAME} STATIC "${${PROJECT_NAME}_SRC}")
target_include_directories (${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_INCLUDE})

minko_enable_plugin_ssl (${PROJECT_NAME})

if (NOT EMSCRIPTEN)
    file (GLOB
        MAIN_INCLUDE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/asio/include"
    )
    file (GLOB_RECURSE
        MAIN_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/websocketpp/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/asio/include/*.hpp"
    )
    target_sources (${PROJECT_NAME} PRIVATE ${MAIN_SRC})
    target_include_directories (${PROJECT_NAME} PRIVATE ${MAIN_INCLUDE})
    target_compile_options (${PROJECT_NAME} PRIVATE "-DASIO_STANDALONE")
endif ()

if (WIN32)
    target_compile_options (${PROJECT_NAME} PRIVATE
        "/bigobj"
        "-D_WEBSOCKETPP_CPP11_INTERNAL_"
        "-D_WIN32_WINNT=0x0501"
    )
endif ()