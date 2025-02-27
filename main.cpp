#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Window size
const int WIDTH = 800;
const int HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Function to generate pixel data
std::vector<unsigned char> generatePixels(int width, int height) {
    std::vector<unsigned char> data(width * height * 3); // RGB format
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int index = (y * width + x) * 3;
            data[index] = x % 256;       // Red
            data[index + 1] = y % 256;   // Green
            data[index + 2] = (x + y) % 256; // Blue
        }
    }
    return data;
}

void renderPixels() {
    std::vector<unsigned char> pixels = generatePixels(WIDTH, HEIGHT);
    glDrawPixels(WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glFlush();
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Metalworks", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        renderPixels();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
