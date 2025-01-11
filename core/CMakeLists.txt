# Specify the minimum CMake version
cmake_minimum_required(VERSION 3.10)


file(COPY ${CMAKE_SOURCE_DIR}/resources/fonts/
     DESTINATION ${CMAKE_BINARY_DIR}/resources/fonts)


file(COPY ${CMAKE_SOURCE_DIR}/targets/
DESTINATION ${CMAKE_BINARY_DIR}/targets)


# Create a library from the core source files
add_library(core STATIC
    src/Window.cpp
    src/Renderer.cpp
    src/InputManager.cpp
    src/Texture.cpp
)

# Specify include directories for the core library
target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}  # Include SDL2 headers
)

# Link SDL2 to the core library if needed
target_link_libraries(core PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(core PRIVATE ${SDL2_TTF_LIBRARIES})