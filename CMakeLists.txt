cmake_minimum_required(VERSION 3.16)
project(ImGuiOpenGL)

set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 REQUIRED)
find_library(OpenGL_LIBRARY OpenGL)

set(IMGUI_SRC
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(ImGuiOpenGL main.cpp ${IMGUI_SRC})

target_include_directories(ImGuiOpenGL PRIVATE external/imgui external/imgui/backends)
target_link_libraries(ImGuiOpenGL glfw ${OpenGL_LIBRARY})
