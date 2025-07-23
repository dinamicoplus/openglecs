cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("LearningOpenGL")

# Collect all .cpp source files recursively from src/
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    #"${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog-1.14.1/src/*.c"
    #"${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog-1.14.1/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/GLAD/src/*.c"
)

add_executable(LearningOpenGL 
    ${SOURCES} 
)

# Specify the include directories you want (add all relevant folders here)
target_include_directories(LearningOpenGL PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/GLAD/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/GLM"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/GLFW"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog-1.14.1/include"
)


# Link import library for DLL
target_link_libraries(LearningOpenGL PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-vc2022/glfw3.lib"
)

# Copy DLL to executable directory after build
add_custom_command(TARGET LearningOpenGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-vc2022/glfw3.dll"
        $<TARGET_FILE_DIR:LearningOpenGL>)

# Copy entire shaders directory to executable directory after build
add_custom_command(TARGET LearningOpenGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders"
        "$<TARGET_FILE_DIR:LearningOpenGL>/shaders")

# Copy entire resources directory to executable directory after build
add_custom_command(TARGET LearningOpenGL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:LearningOpenGL>/resources")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET LearningOpenGL PROPERTY CXX_STANDARD 20)
endif()
