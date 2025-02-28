// Line.cpp
#include "Line.h"
#include <iostream>

Line::Line(const glm::vec3& start, const glm::vec3& end)
        : startPoint(start), endPoint(end), color(1.0f, 1.0f, 1.0f) {

    // Create vertex shader
    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "uniform mat4 MVP;\n"
                                     "void main() {\n"
                                     "   gl_Position = MVP * vec4(aPos, 1.0);\n"
                                     "}\0";

    // Create fragment shader
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec3 lineColor;\n"
                                       "void main() {\n"
                                       "   FragColor = vec4(lineColor, 1.0);\n"
                                       "}\0";

    // Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    updateVertices();
}

Line::~Line() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

void Line::setStartPoint(const glm::vec3& start) {
    startPoint = start;
}

void Line::setEndPoint(const glm::vec3& end) {
    endPoint = end;
}

void Line::setColor(const glm::vec3& newColor) {
    color = newColor;
}

void Line::setMVP(const glm::mat4& mvp) {
    MVP = mvp;
}

glm::vec3 Line::getStartPoint() const {
    return startPoint;
}

glm::vec3 Line::getEndPoint() const {
    return endPoint;
}

glm::vec3 Line::getColor() const {
    return color;
}

void Line::updateVertices() {
    glBindVertexArray(VAO);

    float vertices[] = {
            startPoint.x, startPoint.y, startPoint.z,
            endPoint.x, endPoint.y, endPoint.z
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Line::draw() {
    glUseProgram(shaderProgram);

    // Set uniforms
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &MVP[0][0]);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "lineColor");
    glUniform3fv(colorLoc, 1, &color[0]);

    glBindVertexArray(VAO);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}