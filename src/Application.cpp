#include "Application.h"

#include <spdlog/spdlog.h>

#include <iostream>

Application::Application()
    :
    m_Camera{glm::vec3(0.0f, 0.0f, 3.0f)}, m_FirstMouse{true}, m_Dt{0.0f}, m_LastFrame{0.0f}
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting application...");
    initGLFW();
    initGLAD();

    // Enable Depth testing for Z-Buffer
    glEnable(GL_DEPTH_TEST);

    // Shaders
    m_Shader.create("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    
    m_CubePositions[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    m_CubePositions[1] = glm::vec3(2.0f, 5.0f, -15.0f);
    m_CubePositions[2] = glm::vec3(-1.5f, -2.2f, -2.5f);
    m_CubePositions[3] = glm::vec3(-3.8f, -2.0f, -12.3f);
    m_CubePositions[4] = glm::vec3(2.4f, -0.4f, -3.5f);
    m_CubePositions[5] = glm::vec3(-1.7f, 3.0f, -7.5f);
    m_CubePositions[6] = glm::vec3(1.3f, -2.0f, -2.5f);
    m_CubePositions[7] = glm::vec3(1.5f, 2.0f, -2.5f);
    m_CubePositions[8] = glm::vec3(1.5f, 0.2f, -1.5f);
    m_CubePositions[9] = glm::vec3(-1.3f, 1.0f, -1.5f);

    for (auto &item : m_Cubes)
    {
        item.create();
    }

    m_Shader.use();
    // Set texture uniforms
    m_Shader.setInt("texture1", 0);
    m_Shader.setInt("texture2", 1);

    // MODEL MATRIX
    m_Model = glm::mat4(1.0f);

    // VIEW MATRIX
    // note that we're translating the scene in the reverse direction of where we want to move
    m_View = glm::mat4(1.0f);
    m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));

    // PROJECTION MATRIX
    m_Projection = glm::mat4(1.0f);
    m_Projection = glm::perspective(glm::radians(45.0f), static_cast<float>(s_WindowWidth) / s_WindowHeight, 0.1f, 100.0f);
;
    m_Shader.setMatrix4("view", m_View);
    m_Shader.setMatrix4("projection", m_Projection);

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

    static float value = 0.0f;
    if (glfwGetKey(m_Window, GLFW_KEY_Q))
    {
        value += 0.01f;
    }

    if (glfwGetKey(m_Window, GLFW_KEY_E))
    {
        value -= 0.01f;
    }
    m_Shader.setFloat("value", value);

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(FORWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(BACKWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(LEFT, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(RIGHT, m_Dt);

    //spdlog::debug(value);
    glfwPollEvents();
}

void Application::update()
{
    
    m_Projection = glm::perspective(glm::radians(m_Camera.GetZoom()), static_cast<float>(s_WindowWidth) / s_WindowHeight, 0.1f, 100.0f);
    m_Shader.setMatrix4("projection", m_Projection);
    m_View = m_Camera.GetViewMatrix();
    m_Shader.setMatrix4("view", m_View);

    float currentFrame = static_cast<float>(glfwGetTime());
    m_Dt = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;

}

void Application::render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Transformation and rendering has to be done in the same frame...
    // 
    // Move every cube
    for (unsigned int i = 0; i < 10; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, m_CubePositions[i]);
        float angle = glfwGetTime() * 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m_Cubes[i].applyTransformation(m_Shader, model);
        m_Cubes[i].render();
    }
    m_Shader.use();
    

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

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(m_Window, this);
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
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->framebufferSizeCallback(window, width, height);
            }
            
        });

        
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xposIn, double yposIn)
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->mouse_callback(window, xposIn, yposIn);
            }
        });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
            if (app)
            {
                app->scroll_callback(window, xoffset, yoffset);
            }
        });

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

void Application::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (m_FirstMouse)
    {
        m_LastX = xpos;
        m_LastY = ypos;
        m_FirstMouse = false;
    }

    float xoffset = xpos - m_LastX;
    float yoffset = m_LastY - ypos; // reversed since y-coordinates go from bottom to top

    m_LastX = xpos;
    m_LastY = ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

