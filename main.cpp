/*
 * Real-time Video Processing Assignment
 *
 * FEATURES:
 * - Live camera feed rendering
 * - Multiple filters (Pixelation, Grayscale) with CPU/GPU implementations
 * - Interactive geometric transformations (translate, rotate, scale)
 * - Runtime switching between filters and processing modes
 * - Performance measurement for experimental analysis
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <opencv2/opencv.hpp>

#include <common/Shader.hpp>
#include <common/Camera.hpp>
#include <common/Scene.hpp>
#include <common/Object.hpp>
#include <common/TextureShader.hpp>
#include <common/Quad.hpp>
#include <common/Texture.hpp>

using namespace std;

// --- Global state for interaction ---
enum FilterMode { FILTER_NONE, FILTER_PIXELATE, FILTER_GRAYSCALE };
enum ProcessingMode { CPU_MODE, GPU_MODE };

struct AppState {
    FilterMode currentFilter = FILTER_NONE;
    ProcessingMode processingMode = GPU_MODE;
    
    // Geometric transformation parameters
    glm::vec2 translation = glm::vec2(0.0f, 0.0f);
    float rotation = 0.0f;
    float scale = 1.0f;
    
    // Mouse interaction
    bool isDragging = false;
    glm::vec2 lastMousePos;
    
    // Performance tracking
    std::vector<double> frameTimes;
    int frameCount = 0;
};

AppState appState;

// --- Helper functions ---
bool initWindow(std::string windowName);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

// CPU filter implementations
void applyPixelationCPU(cv::Mat& frame, int pixelSize);
void applyGrayscaleCPU(cv::Mat& frame);

// --- Main ---
int main(void) {
    // --- Step 1: Open camera ---
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera. Exiting." << endl;
        return -1;
    }
    
    // Set camera resolution (can be changed for experimental analysis)
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cout << "Camera opened successfully." << endl;

    // --- Step 2: Initialize OpenGL context ---
    if (!initWindow("Real-time Video Processing")) return -1;

    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize OpenGL context (GLAD)\n");
        cap.release();
        return -1;
    }
    cout << "Loaded OpenGL " << GLVersion.major << "." << GLVersion.minor << "\n";

    // Basic OpenGL setup
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Set up callbacks for interaction
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Create and bind VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // --- Step 3: Prepare Scene, Shaders, and Objects ---
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) {
        cerr << "Error: couldn't capture an initial frame from camera. Exiting.\n";
        cap.release();
        glfwTerminate();
        return -1;
    }

    // Create rendering objects
    TextureShader* textureShader = new TextureShader("shaders/videoTextureShader.vert", "shaders/videoTextureShader.frag");
    Scene* myScene = new Scene();
    Camera* renderingCamera = new Camera();
    renderingCamera->setPosition(glm::vec3(0, 0, -2.5));

    // Create quad with correct aspect ratio
    float videoAspectRatio = (float)frame.cols / (float)frame.rows;
    Quad* myQuad = new Quad(videoAspectRatio);
    myQuad->setShader(textureShader);
    myScene->addObject(myQuad);

    // Create initial texture
    cv::flip(frame, frame, 0);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB); // Convert BGR to RGB
    Texture* videoTexture = new Texture(frame.data, frame.cols, frame.rows, true);
    textureShader->setTexture(videoTexture);

    // --- Step 4: Main Render Loop ---
    cout << "\n=== CONTROLS ===" << endl;
    cout << "1: No filter" << endl;
    cout << "2: Pixelation filter" << endl;
    cout << "3: Grayscale filter" << endl;
    cout << "C: Toggle CPU/GPU processing" << endl;
    cout << "Mouse drag: Translate" << endl;
    cout << "Mouse scroll: Scale" << endl;
    cout << "R: Rotate (hold and drag horizontally)" << endl;
    cout << "Space: Reset transformations" << endl;
    cout << "P: Print performance stats" << endl;
    cout << "ESC: Exit\n" << endl;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- Capture and process frame ---
        cap >> frame;
        if (!frame.empty() && videoTexture != nullptr) {
            cv::Mat processedFrame = frame.clone();
            
            // Apply CPU filters if in CPU mode
            if (appState.processingMode == CPU_MODE) {
                switch (appState.currentFilter) {
                    case FILTER_PIXELATE:
                        applyPixelationCPU(processedFrame, 10);
                        break;
                    case FILTER_GRAYSCALE:
                        applyGrayscaleCPU(processedFrame);
                        break;
                    default:
                        break;
                }
                
                // Apply geometric transformation on CPU
                if (appState.translation != glm::vec2(0.0f) || 
                    appState.rotation != 0.0f || 
                    appState.scale != 1.0f) {
                    
                    cv::Point2f center(processedFrame.cols / 2.0f, processedFrame.rows / 2.0f);
                    cv::Mat transform = cv::getRotationMatrix2D(center, appState.rotation, appState.scale);
                    
                    // Add translation
                    transform.at<double>(0, 2) += appState.translation.x * processedFrame.cols / 2.0f;
                    transform.at<double>(1, 2) -= appState.translation.y * processedFrame.rows / 2.0f;
                    
                    cv::warpAffine(processedFrame, processedFrame, transform, processedFrame.size());
                }
            }
            
            // Update texture
            cv::flip(processedFrame, processedFrame, 0);
            cv::cvtColor(processedFrame, processedFrame, cv::COLOR_BGR2RGB); // Convert BGR to RGB
            videoTexture->update(processedFrame.data, processedFrame.cols, processedFrame.rows, true);
            
            // Set shader uniforms for GPU processing
            if (appState.processingMode == GPU_MODE) {
                textureShader->use();
                
                // Filter parameters
                textureShader->setInt("filterMode", (int)appState.currentFilter);
                textureShader->setInt("pixelSize", 10);
                
                // Transformation parameters
                textureShader->setFloat("uTranslateX", appState.translation.x);
                textureShader->setFloat("uTranslateY", appState.translation.y);
                textureShader->setFloat("uRotation", glm::radians(appState.rotation));
                textureShader->setFloat("uScale", appState.scale);
            } else {
                // In CPU mode, reset GPU transformations to identity
                textureShader->use();
                textureShader->setInt("filterMode", 0); // No GPU filter
                textureShader->setFloat("uTranslateX", 0.0f);
                textureShader->setFloat("uTranslateY", 0.0f);
                textureShader->setFloat("uRotation", 0.0f);
                textureShader->setFloat("uScale", 1.0f);
            } else {
                // In CPU mode, reset GPU transformations to identity
                textureShader->use();
                textureShader->setInt("filterMode", 0); // No GPU filter
                textureShader->setFloat("uTranslateX", 0.0f);
                textureShader->setFloat("uTranslateY", 0.0f);
                textureShader->setFloat("uRotation", 0.0f);
                textureShader->setFloat("uScale", 1.0f);
            }
        }

        // --- Render the scene ---
        myScene->render(renderingCamera);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Track performance
        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameDuration = frameEnd - frameStart;
        appState.frameTimes.push_back(frameDuration.count());
        appState.frameCount++;
        
        // Print FPS every second
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - lastTime;
        if (elapsed.count() >= 1.0) {
            double avgFrameTime = 0;
            for (double t : appState.frameTimes) avgFrameTime += t;
            avgFrameTime /= appState.frameTimes.size();
            double fps = 1000.0 / avgFrameTime;
            
            cout << "FPS: " << fps << " | Mode: " 
                 << (appState.processingMode == GPU_MODE ? "GPU" : "CPU") 
                 << " | Filter: ";
            switch (appState.currentFilter) {
                case FILTER_NONE: cout << "None"; break;
                case FILTER_PIXELATE: cout << "Pixelate"; break;
                case FILTER_GRAYSCALE: cout << "Grayscale"; break;
            }
            cout << endl;
            
            appState.frameTimes.clear();
            lastTime = currentTime;
        }
    }

    // --- Cleanup ---
    cout << "Closing application..." << endl;
    cap.release();
    delete myScene;
    delete renderingCamera;
    delete textureShader;
    delete videoTexture;
    glDeleteVertexArrays(1, &VertexArrayID);
    glfwTerminate();
    return 0;
}

// --- CPU Filter Implementations ---
void applyPixelationCPU(cv::Mat& frame, int pixelSize) {
    for (int y = 0; y < frame.rows; y += pixelSize) {
        for (int x = 0; x < frame.cols; x += pixelSize) {
            cv::Rect roi(x, y, 
                        std::min(pixelSize, frame.cols - x),
                        std::min(pixelSize, frame.rows - y));
            cv::Scalar avgColor = cv::mean(frame(roi));
            frame(roi) = avgColor;
        }
    }
}

void applyGrayscaleCPU(cv::Mat& frame) {
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
}

// --- Callback Functions ---
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_1:
                appState.currentFilter = FILTER_NONE;
                cout << "Filter: None" << endl;
                break;
            case GLFW_KEY_2:
                appState.currentFilter = FILTER_PIXELATE;
                cout << "Filter: Pixelation" << endl;
                break;
            case GLFW_KEY_3:
                appState.currentFilter = FILTER_GRAYSCALE;
                cout << "Filter: Grayscale" << endl;
                break;
            case GLFW_KEY_C:
                appState.processingMode = (appState.processingMode == GPU_MODE) ? CPU_MODE : GPU_MODE;
                cout << "Processing mode: " << (appState.processingMode == GPU_MODE ? "GPU" : "CPU") << endl;
                break;
            case GLFW_KEY_SPACE:
                appState.translation = glm::vec2(0.0f);
                appState.rotation = 0.0f;
                appState.scale = 1.0f;
                cout << "Transformations reset" << endl;
                break;
            case GLFW_KEY_P:
                cout << "\n=== Performance Statistics ===" << endl;
                cout << "Total frames: " << appState.frameCount << endl;
                break;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            appState.isDragging = true;
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            appState.lastMousePos = glm::vec2(xpos, ypos);
        } else if (action == GLFW_RELEASE) {
            appState.isDragging = false;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (appState.isDragging) {
        glm::vec2 currentPos(xpos, ypos);
        glm::vec2 delta = currentPos - appState.lastMousePos;
        
        // Check if R key is held for rotation
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            appState.rotation += delta.x * 0.5f;
        } else {
            // Translation
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            appState.translation.x += delta.x / width * 2.0f;
            appState.translation.y -= delta.y / height * 2.0f;
        }
        
        appState.lastMousePos = currentPos;
    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    appState.scale += yoffset * 0.1f;
    appState.scale = glm::clamp(appState.scale, 0.1f, 5.0f);
}

// --- Helper: initWindow ---
bool initWindow(std::string windowName) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1024, 768, windowName.c_str(), NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    return true;
}