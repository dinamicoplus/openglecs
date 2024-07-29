#include "Application.h"

#include <spdlog/spdlog.h>

#include <iostream>

Application::Application()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting application...");
    initGLFW();
    initGLAD();

    // Shaders
    m_Shader.create("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_Rect.create();
}

Application::~Application()
{
    spdlog::info("Closing application...");

    glfwTerminate();
}

void Application::updateInput()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        close();
    }
    // DEBUG
    if (glfwGetKey(m_Window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        spdlog::debug("Key Enter Pressed!");
    }

    // Toggle on/off wireframe mode
    static bool lock = 0, wf_mode = 0;
    if (!glfwGetKey(m_Window, GLFW_KEY_SPACE)) {
        lock = 0;
    }
    if (glfwGetKey(m_Window, GLFW_KEY_SPACE) && lock == 0) {
        glPolygonMode(GL_FRONT_AND_BACK, (wf_mode = 1 - wf_mode) ? GL_LINE : GL_FILL);
        lock = 1;
    }

    glfwPollEvents();
}

void Application::update()
{
    // update the uniform color
    float timeValue = glfwGetTime();
    float colorValue = sin(timeValue) / 2.0f + 0.5f;
    m_Shader.setFloat3("ourColor", 1.0f - colorValue, colorValue, 0.5f + colorValue);
}

void Application::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_Shader.use();
    m_Rect.render();
    

    glfwSwapBuffers(m_Window);
}

void Application::close()
{
    glfwSetWindowShouldClose(m_Window, true);
}

void Application::initGLFW()
{
    spdlog::info("Initializing GLFW...");
    glfwInit();

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Set core-profile mode
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // Create GLFW window and set some properties
    m_Window = glfwCreateWindow(s_WindowWidth, s_WindowHeight, "OpenGL Testing", nullptr, nullptr);

    // Check for window creation failure
    if (m_Window == nullptr)
    {
        spdlog::critical("Failed to create GFLW window");
        glfwTerminate();
    }

    spdlog::info("GLFW successfully initialized.");

    // Set the window surface as main context
    glfwMakeContextCurrent(m_Window);
}

void Application::initGLAD()
{
    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        spdlog::critical("Failed to initialize GLAD");
        glfwTerminate();
    }
    
    spdlog::info("GLAD successfully initialized.");

    // Set viewport as window size
    glViewport(0, 0, s_WindowWidth, s_WindowHeight);

    // Set callback for window resize
    glfwSetFramebufferSizeCallback(m_Window, framebufferSizeCallback);

    // Show max vertex attrib number
    int numAttrib;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttrib);
    spdlog::info("Maximum vertex attributes supported: {}", numAttrib);

    // Compile shaders
    //compileShaders();
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

