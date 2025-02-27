#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace glm;

class Line {
    GLuint shaderProgram;
    GLuint VBO, VAO;
    std::vector<float> vertices;
    vec3 startPoint, endPoint;
    mat4 MVP;
    vec3 lineColor;

public:
    Line(vec3 start, vec3 end) {
        startPoint = start;
        endPoint = end;
        lineColor = vec3(1.0f, 0.0f, 0.0f); // Red color
        MVP = mat4(1.0f);

        const char *vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec3 aPos;\n"
                                         "uniform mat4 MVP;\n"
                                         "void main() {\n"
                                         "   gl_Position = MVP * vec4(aPos, 1.0);\n"
                                         "}\0";

        const char *fragmentShaderSource = "#version 330 core\n"
                                           "out vec4 FragColor;\n"
                                           "uniform vec3 color;\n"
                                           "void main() {\n"
                                           "   FragColor = vec4(color, 1.0f);\n"
                                           "}\n\0";

        // Compile shaders and link program...
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        vertices = {
                start.x, start.y, start.z,
                end.x, end.y, end.z
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void setMVP(mat4 mvp) {
        MVP = mvp;
    }

    void setColor(vec3 color) {
        lineColor = color;
    }

    void updateVertices() {
        vertices = {
                startPoint.x, startPoint.y, startPoint.z,
                endPoint.x, endPoint.y, endPoint.z
        };

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

    ~Line() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }

    void setStartPoint(vec3 point) { startPoint = point; }
    void setEndPoint(vec3 point) { endPoint = point; }
    vec3 getStartPoint() { return startPoint; }
    vec3 getEndPoint() { return endPoint; }
};

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return -1;

    // Set OpenGL version to 4.1 (macOS max version)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Line Drawing with ImGui", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize OpenGL and ImGui
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Define projection and model-view matrices
    mat4 projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f); // Orthographic projection
    mat4 view = mat4(1.0f);
    mat4 model = mat4(1.0f);
    mat4 MVP = projection * view * model;

    // Create the line with initial points
    Line line(vec3(-0.5f, 0.0f, 0.0f), vec3(0.5f, 0.0f, 0.0f));
    line.setMVP(MVP);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create the ImGui UI for modifying line points
        ImGui::Begin("Line Settings");
        ImGui::Text("Use the sliders to set the start and end points of the line");

        vec3 start = line.getStartPoint();
        vec3 end = line.getEndPoint();

        if (ImGui::SliderFloat3("Start Point", &start.x, -1.0f, 1.0f)) {
            line.setStartPoint(start);
            line.updateVertices();
        }
        if (ImGui::SliderFloat3("End Point", &end.x, -1.0f, 1.0f)) {
            line.setEndPoint(end);
            line.updateVertices();
        }

        ImGui::End();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the line
        line.draw();

        // Render ImGui UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
