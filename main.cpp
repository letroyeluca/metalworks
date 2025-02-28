// main.cpp
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include "external/glm/glm/glm.hpp"
#include "external/glm/glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Line.h"
#include "ini_configuration.h"
#include "l_parser.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cmath>
#include <vector>
#include <stack>
#include <set>




using namespace glm;

// Structure to hold line data
struct LineData {
    vec3 start;
    vec3 end;
    vec3 color;
};

// Global variables
std::vector<LineData> linesData;
std::string currentRenderType = "None";
ini::Configuration currentConfig;
bool configLoaded = false;

// Function prototypes
void glfw_error_callback(int error, const char* description);
GLFWwindow* initializeOpenGL();
void initializeImGui(GLFWwindow* window);
void renderRectangle(const ini::Configuration &conf);
void renderBlocks(const ini::Configuration &conf);
void renderL2D(const ini::Configuration &conf);
void renderScene(const ini::Configuration &conf);
void loadConfiguration(const std::string& fileName);

void glfw_error_callback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

GLFWwindow* initializeOpenGL() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Renderer with ImGui", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return window;
}

void initializeImGui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

// Function to load and parse INI configuration
void loadConfiguration(const std::string& fileName) {
    try {
        std::ifstream fin(fileName);
        if (fin.peek() == std::istream::traits_type::eof()) {
            std::cout << "Ini file appears empty. Does '" << fileName << "' exist?" << std::endl;
            configLoaded = false;
            return;
        }
        fin >> currentConfig;
        fin.close();
        configLoaded = true;

        // Get render type
        currentRenderType = currentConfig["General"]["type"].as_string_or_die();
        std::cout << "Loaded configuration with type: " << currentRenderType << std::endl;
    }
    catch(ini::ParseException& ex) {
        std::cerr << "Error parsing file: " << fileName << ": " << ex.what() << std::endl;
        configLoaded = false;
    }
}

// Creates lines for a colored rectangle
void renderRectangle(const ini::Configuration &conf) {
    linesData.clear();

    int width = (conf["ImageProperties"]["width"].as_int_or_die());
    int height = (conf["ImageProperties"]["height"].as_int_or_die());

    // For a rectangle, we'll just create the border lines
    LineData line1, line2, line3, line4;

    // Define rectangle corners
    vec3 topLeft(-0.8f, 0.8f, 0.0f);
    vec3 topRight(0.8f, 0.8f, 0.0f);
    vec3 bottomRight(0.8f, -0.8f, 0.0f);
    vec3 bottomLeft(-0.8f, -0.8f, 0.0f);

    // Define colors
    vec3 redColor(1.0f, 0.0f, 0.0f);
    vec3 greenColor(0.0f, 1.0f, 0.0f);
    vec3 blueColor(0.0f, 0.0f, 1.0f);
    vec3 yellowColor(1.0f, 1.0f, 0.0f);

    // Create the four sides of the rectangle
    line1.start = topLeft;
    line1.end = topRight;
    line1.color = redColor;

    line2.start = topRight;
    line2.end = bottomRight;
    line2.color = greenColor;

    line3.start = bottomRight;
    line3.end = bottomLeft;
    line3.color = blueColor;

    line4.start = bottomLeft;
    line4.end = topLeft;
    line4.color = yellowColor;

    // Add lines to the global vector
    linesData.push_back(line1);
    linesData.push_back(line2);
    linesData.push_back(line3);
    linesData.push_back(line4);
}

// Creates lines for a checkerboard pattern
void renderBlocks(const ini::Configuration &conf) {
    linesData.clear();

    int nrXBlocks = (conf["BlockProperties"]["nrXBlocks"].as_int_or_die());
    int nrYBlocks = (conf["BlockProperties"]["nrYBlocks"].as_int_or_die());
    std::vector<double> colorA = (conf["BlockProperties"]["colorWhite"].as_double_tuple_or_die());
    std::vector<double> colorB = (conf["BlockProperties"]["colorBlack"].as_double_tuple_or_die());

    // Convert the configuration colors to vec3
    vec3 colorAvec(colorA[0], colorA[1], colorA[2]);
    vec3 colorBvec(colorB[0], colorB[1], colorB[2]);

    // Create a grid of lines to represent the checkerboard
    float blockWidth = 1.6f / nrXBlocks;
    float blockHeight = 1.6f / nrYBlocks;
    float startX = -0.8f;
    float startY = -0.8f;

    // Create horizontal grid lines
    for (int y = 0; y <= nrYBlocks; y++) {
        LineData line;
        line.start = vec3(startX, startY + y * blockHeight, 0.0f);
        line.end = vec3(startX + nrXBlocks * blockWidth, startY + y * blockHeight, 0.0f);
        line.color = vec3(0.5f, 0.5f, 0.5f); // Gray lines
        linesData.push_back(line);
    }

    // Create vertical grid lines
    for (int x = 0; x <= nrXBlocks; x++) {
        LineData line;
        line.start = vec3(startX + x * blockWidth, startY, 0.0f);
        line.end = vec3(startX + x * blockWidth, startY + nrYBlocks * blockHeight, 0.0f);
        line.color = vec3(0.5f, 0.5f, 0.5f); // Gray lines
        linesData.push_back(line);
    }
}

// Renders an L-System 2D drawing
void renderL2D(const ini::Configuration &conf) {
    linesData.clear();

    int size = (conf["General"]["size"].as_int_or_die());
    std::vector<double> backgroundColor = (conf["General"]["backgroundcolor"].as_double_tuple_or_die());
    std::vector<double> lineColor = (conf["2DLSystem"]["color"].as_double_tuple_or_die());
    std::string L2DFileName = (conf["2DLSystem"]["inputfile"].as_string_or_die());

    // Convert the line color to vec3
    vec3 lineColorVec(lineColor[0], lineColor[1], lineColor[2]);

    // Load L-System
    LParser::LSystem2D LPARSER;
    std::ifstream L2DFile(L2DFileName);
    if (!L2DFile) {
        std::cerr << "Failed to open L-System file: " << L2DFileName << std::endl;
        return;
    }

    L2DFile >> LPARSER;
    L2DFile.close();

    std::set<char> alphabet = LPARSER.get_alphabet();

    // Generate the L-System string
    std::string mainstring = LPARSER.get_initiator();
    std::string tempstring;
    for(int i = 0; i < LPARSER.get_nr_iterations(); i++) {
        tempstring = "";
        for(char c: mainstring) {
            if(alphabet.find(c) != alphabet.end()) {
                tempstring += LPARSER.get_replacement(c);
            } else {
                tempstring += c;
            }
        }
        mainstring = tempstring;
    }

    // Trace the path to collect line segments
    double currentX = 0;
    double currentY = 0;
    double nextX = 0;
    double nextY = 0;
    double currentAngle = LPARSER.get_starting_angle() * (M_PI / 180);
    double angle = LPARSER.get_angle() * (M_PI / 180);

    std::stack<double> positionX;
    std::stack<double> positionY;
    std::stack<double> positionAngle;

    for(char c: mainstring) {
        if(alphabet.find(c) != alphabet.end()) {
            if(LPARSER.draw(c) != false) {
                nextX = currentX + LPARSER.draw(c) * cos(currentAngle);
                nextY = currentY + LPARSER.draw(c) * sin(currentAngle);

                LineData line;
                line.start = vec3(currentX, currentY, 0.0f);
                line.end = vec3(nextX, nextY, 0.0f);
                line.color = lineColorVec;
                linesData.push_back(line);

                currentX = nextX;
                currentY = nextY;
            } else {
                currentX = currentX + LPARSER.draw(c) * cos(currentAngle);
                currentY = currentY + LPARSER.draw(c) * sin(currentAngle);
            }
        }
        if(c == '+') {
            currentAngle += angle;
        }
        else if(c == '-') {
            currentAngle -= angle;
        } else if(c == '(') {
            positionX.push(currentX);
            positionY.push(currentY);
            positionAngle.push(currentAngle);
        } else if(c == ')') {
            currentX = positionX.top();
            currentY = positionY.top();
            currentAngle = positionAngle.top();
            positionX.pop();
            positionY.pop();
            positionAngle.pop();
        }
    }

    // Normalize coordinates
    if (!linesData.empty()) {
        // Find min and max
        float minX = linesData[0].start.x;
        float maxX = minX;
        float minY = linesData[0].start.y;
        float maxY = minY;

        for (const auto& line : linesData) {
            minX = std::min(minX, std::min(line.start.x, line.end.x));
            maxX = std::max(maxX, std::max(line.start.x, line.end.x));
            minY = std::min(minY, std::min(line.start.y, line.end.y));
            maxY = std::max(maxY, std::max(line.start.y, line.end.y));
        }

        // Calculate scale factor
        float width = maxX - minX;
        float height = maxY - minY;
        float scale = 1.6f / std::max(width, height);

        // Center and scale
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;

        for (auto& line : linesData) {
            line.start.x = (line.start.x - centerX) * scale;
            line.start.y = (line.start.y - centerY) * scale;
            line.end.x = (line.end.x - centerX) * scale;
            line.end.y = (line.end.y - centerY) * scale;
        }
    }
}

// Main render function that calls the appropriate renderer
void renderScene(const ini::Configuration &conf) {
    if (conf["General"]["type"].as_string_or_die() == "IntroColorRectangle") {
        renderRectangle(conf);
    }
    else if (conf["General"]["type"].as_string_or_die() == "IntroBlocks") {
        renderBlocks(conf);
    }
    else if (conf["General"]["type"].as_string_or_die() == "2DLSystem") {
        renderL2D(conf);
    }
}

int main(int argc, char* argv[]) {
    GLFWwindow* window = initializeOpenGL();
    if (!window) return -1;

    initializeImGui(window);

    // Set up projection matrix
    mat4 projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    mat4 view = mat4(1.0f);
    mat4 model = mat4(1.0f);
    mat4 MVP = projection * view * model;

    // Default line (will be replaced when configuration is loaded)
    Line defaultLine(vec3(-0.5f, 0.0f, 0.0f), vec3(0.5f, 0.0f, 0.0f));
    defaultLine.setMVP(MVP);

    // Vector to store Line objects
    std::vector<Line> lines;

    // Process command line arguments
    std::vector<std::string> fileArgs;
    if (argc > 1) {
        fileArgs = std::vector<std::string>(argv + 1, argv + argc);
    } else {
        // Try to read from filelist if no arguments provided
        std::ifstream fileIn("filelist");
        std::string filelistName;
        while (std::getline(fileIn, filelistName)) {
            fileArgs.push_back(filelistName);
        }
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui windows
        ImGui::Begin("Renderer Controls");

        static char iniFilePath[256] = "";
        ImGui::InputText("INI File Path", iniFilePath, IM_ARRAYSIZE(iniFilePath));

        if (ImGui::Button("Load Configuration")) {
            loadConfiguration(iniFilePath);
            if (configLoaded) {
                renderScene(currentConfig);

                // Create Line objects from lineData
                lines.clear();
                for (const auto& data : linesData) {
                    Line line(data.start, data.end);
                    line.setMVP(MVP);
                    line.setColor(data.color);
                    lines.push_back(line);
                }
            }
        }

        ImGui::Text("Current Render Type: %s", currentRenderType.c_str());
        ImGui::Text("Number of Lines: %zu", lines.size());

        // Controls for camera/view
        static float zoom = 1.0f;
        static float panX = 0.0f;
        static float panY = 0.0f;

        if (ImGui::SliderFloat("Zoom", &zoom, 0.1f, 5.0f)) {
            projection = ortho(-1.0f/zoom, 1.0f/zoom, -1.0f/zoom, 1.0f/zoom, -1.0f, 1.0f);
            MVP = projection * view * model;

            for (auto& line : lines) {
                line.setMVP(MVP);
            }
        }

        if (ImGui::SliderFloat("Pan X", &panX, -2.0f, 2.0f) ||
            ImGui::SliderFloat("Pan Y", &panY, -2.0f, 2.0f)) {

            view = translate(mat4(1.0f), vec3(panX, panY, 0.0f));
            MVP = projection * view * model;

            for (auto& line : lines) {
                line.setMVP(MVP);
            }
        }

        ImGui::End();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw all lines
        for (auto& line : lines) {
            line.draw();
        }

        // If no lines loaded yet, show default line
        if (lines.empty()) {
            defaultLine.draw();
        }

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