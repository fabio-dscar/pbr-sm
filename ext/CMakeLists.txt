cmake_minimum_required(VERSION 3.20)

# ---------------------------------------------------------------------------------------
# glad
# ---------------------------------------------------------------------------------------
set(GLAD_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include PARENT_SCOPE)
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c)

target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

set_property(TARGET glad PROPERTY FOLDER "ext")

# ---------------------------------------------------------------------------------------
# tinyobj
# ---------------------------------------------------------------------------------------
set(TINYOBJ_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/tinyobj PARENT_SCOPE)

# ---------------------------------------------------------------------------------------
# Imgui
# ---------------------------------------------------------------------------------------
set(IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp PARENT_SCOPE
)

# ---------------------------------------------------------------------------------------
# glfw
# ---------------------------------------------------------------------------------------
set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)

add_subdirectory(glfw)

set(GLFW_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include PARENT_SCOPE)
set_property(TARGET glfw PROPERTY FOLDER "ext")

# ---------------------------------------------------------------------------------------
# spdlog
# ---------------------------------------------------------------------------------------
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL " " FORCE)

add_subdirectory(spdlog)

set(SPDLOG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/spdlog/include PARENT_SCOPE)
set_property(TARGET spdlog PROPERTY FOLDER "ext")

# ---------------------------------------------------------------------------------------
# backward-cpp
# ---------------------------------------------------------------------------------------
add_subdirectory(backward-cpp)

set(BACKWARD_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/backward-cpp PARENT_SCOPE)
set_property(TARGET spdlog PROPERTY FOLDER "ext")
