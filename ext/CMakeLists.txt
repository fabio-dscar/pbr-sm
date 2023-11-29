cmake_minimum_required (VERSION 3.20)

#########################################
## zlib
find_package(ZLIB)
if(NOT ZLIB_FOUND)
    set(ZLIB_BUILD_STATIC_LIBS ON CACHE BOOL " " FORCE)
    set(ZLIB_BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
    add_subdirectory(zlib)

    set(ZLIB_LIBRARIES zlibstatic)
    set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib)

    add_library(ZLIB::ZLIB ALIAS zlibstatic)
    include_directories(${ZLIB_INCLUDE_DIRS})
else()
    message(VERBOSE ${ZLIB_INCLUDE_DIRS})
    message(VERBOSE ${ZLIB_LIBRARIES})
endif()

set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS} PARENT_SCOPE)
set(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} PARENT_SCOPE)

#########################################
## glad
set(GLAD_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include PARENT_SCOPE)
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c)

target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

set_property(TARGET glad PROPERTY FOLDER "ext")

#########################################
## tinyobj
set(TINYOBJ_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/tinyobj PARENT_SCOPE)

#########################################
## Imgui
set(IMGUI_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp
    PARENT_SCOPE)

#########################################
## tinygltf
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL " " FORCE)
set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL " " FORCE)
set(TINYGLTF_BUILD_VALIDATOR_EXAMPLE OFF CACHE BOOL " " FORCE)
set(TINYGLTF_BUILD_BUILDER_EXAMPLE OFF CACHE BOOL " " FORCE)
set(TINYGLTF_INSTALL OFF CACHE BOOL " " FORCE)
set(TINYGLTF_HEADER_ONLY ON CACHE BOOL " " FORCE)

add_subdirectory(tinygltf)

#########################################
## glfw
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)

add_subdirectory(glfw)

set(GLFW_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include PARENT_SCOPE)
set_property(TARGET glfw PROPERTY FOLDER "ext")