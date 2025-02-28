// Line.h
#ifndef LINE_H
#define LINE_H

#include "external/glm/glm/glm.hpp"
#include <OpenGL/gl3.h>

class Line {
private:
    glm::vec3 startPoint;
    glm::vec3 endPoint;
    glm::vec3 color;
    GLuint VAO, VBO;
    GLuint shaderProgram;
    glm::mat4 MVP;

public:
    Line(const glm::vec3& start, const glm::vec3& end);
    ~Line();

    void setStartPoint(const glm::vec3& start);
    void setEndPoint(const glm::vec3& end);
    void setColor(const glm::vec3& color);
    void setMVP(const glm::mat4& mvp);

    glm::vec3 getStartPoint() const;
    glm::vec3 getEndPoint() const;
    glm::vec3 getColor() const;

    void updateVertices();
    void draw();
};

#endif // LINE_H