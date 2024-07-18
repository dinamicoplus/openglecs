#include "Application.h"

#include "ShaderProgram.h"

#include <spdlog/spdlog.h>

#include <iostream>

Application::Application()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting application...");
    initGLFW();
    initGLAD();

    ShaderProgram::create("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    rect.create();
}

Application::~Application()
{
    spdlog::info("Closing application...");
    
    ShaderProgram::destroy();

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

}

void Application::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ShaderProgram::use();
    rect.render();
    

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

void Application::compileShaders()
{
    // Vertex shader
    const char* vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int  success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        spdlog::error("Vertex Shader compilation failed: ", infoLog);
    }

    // Fragment shader
    const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(0.5f, 1.0f, 0.0f, 1.0f);\n"
        "}\n\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        spdlog::error("Fragment Shader compilation failed : {}", infoLog);
    }


    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertexShader);
    glAttachShader(m_ShaderProgram, fragmentShader);
    glLinkProgram(m_ShaderProgram);

    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
        spdlog::error("Shader program linking failed: {}", infoLog);
    }

    glUseProgram(m_ShaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}
