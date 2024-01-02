cmake_minimum_required(VERSION 3.20)

project(pbr-sm VERSION 0.1 LANGUAGES CXX C)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

##################################
## Third party libs
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ext)

##################################
## OpenGL
find_package(OpenGL REQUIRED)

set(RT_PBR_SOURCES
  src/App/OpenGLApplication.cpp
  src/App/PBRApp.cpp

  src/Core/Camera.cpp
  src/Core/Geometry.cpp
  src/Core/Mesh.cpp
  src/Core/Perspective.cpp
  src/Core/Resources.cpp
  src/Core/Scene.cpp
  src/Core/SceneObject.cpp
  src/Core/Shape.cpp
  src/Core/Skybox.cpp
  src/Core/Spectrum.cpp
  src/Core/Sphere.cpp
  src/Core/Quad.cpp
  src/Core/Texture.cpp

  src/Graphics/Renderer.cpp
  src/Graphics/RenderInterface.cpp
  src/Graphics/Buffer.cpp
  src/Graphics/Shader.cpp

  src/GUI/GUI.cpp

  src/Lights/DirectionalLight.cpp
  src/Lights/Light.cpp
  src/Lights/PointLight.cpp
  src/Lights/SpotLight.cpp
  src/Lights/SphereLight.cpp
  src/Lights/TubeLight.cpp

  src/Materials/Material.cpp
  src/Materials/PBRMaterial.cpp

  src/Math/Bounds.cpp
  src/Math/Hash.cpp
  src/Math/Matrix2x2.cpp
  src/Math/Matrix3x3.cpp
  src/Math/Matrix4x4.cpp
  src/Math/Quat.cpp
  src/Math/Ray.cpp
  src/Math/Transform.cpp
  src/Math/Vector2.cpp
  src/Math/Vector3.cpp
  src/Math/Vector4.cpp

  src/Utils/Image.cpp
  src/Utils/LoadXML.cpp
  src/Utils/ParameterMap.cpp
  src/Utils/Utils.cpp
  src/Utils/Log.cpp

  src/Loaders/ObjLoader.cpp
  src/Loaders/GltfLoader.cpp
  
  ext/pugixml/pugixml.cpp
  ext/lodepng/lodepng.cpp
  ext/mikktspace/mikktspace.c
  ${IMGUI_SOURCES}
)

add_executable(pbr-sm src/main.cpp ${RT_PBR_SOURCES})
target_compile_features(pbr-sm PUBLIC cxx_std_20)
target_compile_definitions(pbr-sm PUBLIC "$<$<CONFIG:Debug>:DEBUG>")
target_include_directories(pbr-sm PUBLIC
  src
  src/App
  src/Core
  src/Loaders
  src/Graphics
  src/GUI
  src/Lights
  src/Materials
  src/Math
  src/Utils
  ext  
  ext/imgui
  ext/lodepng
  ext/pugixml
  ext/mikktspace
  ext/tinygltf
  ${TINYOBJ_INCLUDE}
  ${ZLIB_INCLUDE_DIRS}
  ${GLAD_INCLUDE}
  ${GLFW_INCLUDE_DIRS}
  ${SPDLOG_INCLUDE_DIRS}
  ${BACKWARD_INCLUDE_DIRS}
)
target_link_libraries(pbr-sm PRIVATE
  OpenGL::GL
  glad
  glfw
  Backward::Interface
  spdlog
  ${ZLIB_LIBRARIES}
)

add_custom_command(TARGET pbr-sm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data/Shaders ${CMAKE_BINARY_DIR}/glsl
)

set(RELEASE_FLAGS "-march=native" "-mtune=native") # "-flto")
target_compile_options(pbr-sm PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")