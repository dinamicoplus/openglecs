#include "Application.h"

#include <spdlog/spdlog.h>

#include <iostream>

Application::Application()
    :
    m_Camera{glm::vec3(10.0f, 10.0f, 60.0f)}, m_FirstMouse{true}, m_Dt{0.0f}, m_LastFrame{0.0f}
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting application...");

    initGLFW();
    initGLAD();

    // Enable Depth testing for Z-Buffer
    glEnable(GL_DEPTH_TEST);

    float vertices[] = {
        //POSITION             //COLOR             //TEXTCOORDS  //NORMALS
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f
    };


    scene.RegisterComponentType<TexturedModelComponent>();
    EntityID newEnt = scene.NewEntity();
    TexturedModelComponent* model = scene.AssignComponent<TexturedModelComponent>(newEnt);
    ModelManager::initModel(*model);
	ModelManager::initModelIntoGPU(*model);
    ModelManager::loadDataIntoModel(*model, vertices, sizeof(vertices)/sizeof(float));
	ModelManager::loadModelIntoGPU(*model, false); // Load data into GPU and remove from memory
    
    //spdlog::info("TexturedModelComponent ID: {}", scene.GetComponentId<TexturedModelComponent>());
    // En el inspection *(TexturedModelComponent*)scene.m_ComponentPools[0]->get(0)

    // Shaders
    m_Shader.create("shaders/vertex.glsl", "shaders/fragment.glsl");
    m_Shader.use();
    
    // Textures
    unsigned int containerTexture = TextureManager::create("resources/container.jpg");
    unsigned int wallTexture = TextureManager::create("resources/wall.jpg");

    model->m_TextureID = TextureManager::get(containerTexture);

    spdlog::info("Light position: x={0:.4f}, y={0:.4f}, z={0:.4f}", (float)lightPos.x, (float)lightPos.y, (float)lightPos.z);

    // Create awesome cube grid
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

  //  for (auto& cube : m_Cubes)
  //  {
  //      cube.create(m_Shader, glm::vec3(x, y, z));
  //      cube.setTexture(containerTexture);
  //      
  //      
  //      x += 1.0f;
  //      if (x == 10.0f) 
  //      {
  //          cube.setTexture(wallTexture);
  //          z += 1.0f;
  //          x = 0.0f;
  //      }
  //      if (z == 10.0f)
  //      {
  //          cube.setTexture(0);
  //          y += 1.0f;
  //          z = 0.0f;
  //      }
  //  }
    lightcube.create(m_Shader, lightPos);

    // MODEL MATRIX
    m_Model = glm::mat4(1.0f);

    // VIEW MATRIX
    // note that we're translating the scene in the reverse direction of where we want to move
    m_View = glm::mat4(1.0f);
    m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
    
    // PROJECTION MATRIX
    m_AspectRatio = static_cast<float>(s_WindowWidth) / s_WindowHeight;
    m_Projection = glm::mat4(1.0f);
    m_Projection = glm::perspective(glm::radians(45.0f), m_AspectRatio, 0.1f, 100.0f);
;
    m_Shader.setMatrix4("view", m_View);
    m_Shader.setMatrix4("projection", m_Projection);

}

Application::~Application()
{
    spdlog::info("Closing application...");
    // Delete textures
    TextureManager::freeAll();
    glfwTerminate();
}

void Application::updateInput()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        close();
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

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(FORWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(BACKWARD, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(LEFT, m_Dt);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(RIGHT, m_Dt);

        

    /*
    if (glfwGetKey(m_Window, GLFW_KEY_UP) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(0.0f, 0.0f, m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(0.0f, 0.0f, -m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_LEFT) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(-m_Dt, 0.0f, 0.0f));
    if (glfwGetKey(m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        m_Cube.translate(m_Shader, glm::vec3(m_Dt, 0.0f, 0.0f));

    if (glfwGetKey(m_Window, GLFW_KEY_Z) == GLFW_PRESS)
        m_Cube.rotate(m_Shader, m_Dt, glm::vec3(1.0f, 1.0f, 0.0f));
    if (glfwGetKey(m_Window, GLFW_KEY_C) == GLFW_PRESS)
        m_Cube.rotate(m_Shader, -m_Dt, glm::vec3(1.0f, 1.0f, 0.0f));

    if (glfwGetKey(m_Window, GLFW_KEY_F) == GLFW_PRESS)
        m_Cube.scale(m_Shader, glm::vec3(1.0f - m_Dt));
    if (glfwGetKey(m_Window, GLFW_KEY_G) == GLFW_PRESS)
        m_Cube.scale(m_Shader, glm::vec3(1.0f + m_Dt));*/

    
    glfwPollEvents();
}

void Application::update()
{

    m_Projection = glm::perspective(glm::radians(m_Camera.GetZoom()), m_AspectRatio, 0.1f, 100.0f);
    m_Shader.setMatrix4("projection", m_Projection);
    m_View = m_Camera.GetViewMatrix();
    m_Shader.setMatrix4("view", m_View);

    //spdlog::info("Camera position: x={0:.4f}, y={0:.4f}, z={0:.4f}", (float)m_Camera.m_Position.x, (float)m_Camera.m_Position.y, (float)m_Camera.m_Position.z);
    lightPos.x = sin(glfwGetTime()) * radius + 5.0f;
    lightPos.y = cos(glfwGetTime()) * radius + 5.0f;

    lightColor.r = sin(glfwGetTime());
    lightColor.g = sin(10 / 11 * glfwGetTime() + 3.141592f / 3.0f);
    lightColor.b = sin(5 / 6 * glfwGetTime() + 3.141592f * 2.0f / 3.0f);

    lightcube.setPosition(lightPos);
    m_Shader.setVec3("lightPos", lightPos);
    m_Shader.setVec3("ambientColor", lightColor);
    m_Shader.setVec3("viewPos", m_Camera.m_Position);

    // Update DT
    float currentFrame = static_cast<float>(glfwGetTime());
    m_Dt = currentFrame - m_LastFrame;
    m_LastFrame = currentFrame;
}

void Application::render()
{
    glClearColor(lightColor.r, lightColor.g, lightColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ComponentArray<TexturedModelComponent>* TMC = scene.GetComponentArray<TexturedModelComponent>();
    for (auto& model : TMC->GetComponents())
    {
		RenderSystem::render(&model, m_Shader);
	}

    //for (auto& cube : m_Cubes)
    //{
    //    cube.render(m_Shader);
    //}
    lightcube.render(m_Shader);

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

    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);


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
    int fbW, fbH;
    glfwGetFramebufferSize(m_Window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);
    m_AspectRatio = static_cast<float>(fbW) / fbH;   // keep your aspect ratio in sync
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

