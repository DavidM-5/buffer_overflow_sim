# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)

# ------------------- Main Application -------------------

# Create an executable from the application source files
add_executable(buffer_overflow_sim
    src/main.cpp
    src/Application.cpp
    src/BOApplication/BOApplication.cpp
    src/Widget.cpp
    src/Panel.cpp
    src/TextBlock.cpp
    src/Border.cpp
    src/StackVisualizer.cpp
    src/TextLine.cpp
    src/Console.cpp
    src/BOApplication/ConsoleProccess.cpp
    src/BOApplication/GDBController.cpp
    src/Button.cpp
    src/Paragraph.cpp
)

# Set the output directory for the executable
set_target_properties(buffer_overflow_sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build  # Output to project-root
)

# Link the core library and SDL2 to the application
target_link_libraries(buffer_overflow_sim PRIVATE core ${SDL2_LIBRARIES})

# Specify include directories for the application
target_include_directories(buffer_overflow_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}  # Include SDL2 headers
)


# ------------------- BOApplication Standalone Executable -------------------

add_executable(BOAppExec
    src/BOApplication/Main.cpp               # Create this file
    src/BOApplication/BOApplication.cpp
    src/BOApplication/ConsoleProccess.cpp
    src/BOApplication/GDBController.cpp
    src/Widget.cpp
    src/Panel.cpp
    src/TextBlock.cpp
    src/Border.cpp
    src/StackVisualizer.cpp
    src/TextLine.cpp
    src/Console.cpp
    src/Button.cpp
    src/Paragraph.cpp
)

set_target_properties(BOAppExec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

target_link_libraries(BOAppExec PRIVATE core ${SDL2_LIBRARIES})
target_include_directories(BOAppExec PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
)
