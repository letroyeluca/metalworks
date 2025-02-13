cmake_minimum_required(VERSION 3.29)
project(metalworks)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

# Manually set GLFW paths if not found automatically
set(GLFW3_DIR "/usr/local/Cellar/glfw/3.4")  # Adjust version if needed
set(GLFW_INCLUDE_DIR "${GLFW3_DIR}/include")
set(GLFW_LIBRARY "${GLFW3_DIR}/lib/libglfw.dylib")

add_executable(metalworks main.cpp)

target_include_directories(metalworks PRIVATE ${GLFW_INCLUDE_DIR})
target_link_libraries(metalworks PRIVATE ${OPENGL_gl_LIBRARY} ${GLFW_LIBRARY})
