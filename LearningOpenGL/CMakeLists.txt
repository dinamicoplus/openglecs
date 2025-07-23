cmake_minimum_required (VERSION 3.10)

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

# Detect the build host’s platform/arch and link the correct GLFW library
if (WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)        # 64-bit Windows
    target_link_libraries(LearningOpenGL PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-vc2022/glfw3.lib")

    # Copy DLL next to the executable after linking
    add_custom_command(TARGET LearningOpenGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-vc2022/glfw3.dll"
            $<TARGET_FILE_DIR:LearningOpenGL>)


elseif(APPLE AND CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")   # macOS on M-series
    # --- Link the dylib ---
    target_link_libraries(LearningOpenGL PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-arm64/libglfw.3.dylib")

    # --- Make sure the dylib is next to the executable so the loader finds it ---
    add_custom_command(TARGET LearningOpenGL POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/lib-arm64/libglfw.3.dylib"
            $<TARGET_FILE_DIR:LearningOpenGL>)

    # --- (Optional but recommended) Set an rpath so the app can also find it
    #     via @loader_path at runtime without copying ---
    set_target_properties(LearningOpenGL PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path")
endif()

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
