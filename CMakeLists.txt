cmake_minimum_required(VERSION 3.22)

project(SR2 VERSION 1.0)

# Enable C++20 features
set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

# Set SDL2_DIR if SDL2 is installed in a non-standard location
set(SDL2_DIR "C:/Develop/SDL2/cmake") # Replace with the actual path

# Find SDL2
find_package(SDL2 REQUIRED PATHS "C:/Develop/SDL2/")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${SOURCES}
)

# Link against SDL2 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)

# Include GLM headers
target_include_directories(${PROJECT_NAME} PRIVATE "C:/Develop/glm")
