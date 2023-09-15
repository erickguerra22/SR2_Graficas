#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <Windows.h>
#include "color.h"
#include "framebuffer.h"
#include "objReader.h"
#include "uniforms.h"
#include "shaders.h"
#include "triangle.h"
#include "camera.h"
#include "primitiveAssembly.h"
#include "rasterize.h"
#include "matrixes.h"
#include "fragment.h"

const float SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const float SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
Framebuffer framebuffer = Framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

Color clearColor = Color(0, 0, 0);
glm::vec3 light = glm::vec3(0.0f, 0.0f, 1.0f);
Color mainColor = Color(255, 255, 255);
const std::string modelPath = "../models/test.obj";
float rotationAngle = glm::radians(0.0f);

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<Face> faces;
std::vector<Vertex> verticesArray;

Uniforms uniforms;

glm::mat4 model = glm::mat4(1);
glm::mat4 view = glm::mat4(1);
glm::mat4 projection = glm::mat4(1);

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Error: No se puedo inicializar SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SR 2: Flat Shading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Error: No se pudo crear una ventana SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Error: No se pudo crear SDL_Renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void render(Primitive polygon)
{

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 1. Vertex Shader
    std::vector<Vertex> transformedVertices;

    for (const Vertex &vertex : verticesArray)
    {
        transformedVertices.push_back(vertexShader(vertex, uniforms));
    }

    // 2. Primitive Assembly
    std::vector<std::vector<Vertex>> assembledVertices = primitiveAssembly(polygon, transformedVertices);

    // 3. Rasterization
    std::vector<Fragment> fragments = rasterize(polygon, assembledVertices, SCREEN_WIDTH, SCREEN_HEIGHT, framebuffer.getLight(), framebuffer.getMainColor());

    // 4. Fragment Shader
    for (Fragment &fragment : fragments)
    {
        // Apply the fragment shader to compute the final color
        fragment = fragmentShader(fragment);
        framebuffer.point(fragment);
    }
}

int main(int argv, char **args)
{
    framebuffer.setClearColor(clearColor);
    framebuffer.setMainColor(mainColor);
    framebuffer.setLight(light);

    if (!init())
    {
        return 1;
    }

    framebuffer.clear();

    loadOBJ(modelPath, vertices, normals, faces);
    verticesArray = setupVertexArray(vertices, normals, faces);

    glm::vec3 translationVector(0.0f, 0.0f, 0.0f);
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotar alrededor del eje Y
    glm::vec3 scaleFactor(1.0f, 1.0f, 1.0f);

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), translationVector);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), scaleFactor);

    // Inicializar cámara
    Camera camera;
    camera.cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    camera.targetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Matriz de proyección
    uniforms.projection = createProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Matriz de viewport
    uniforms.viewport = createViewportMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Matriz de vista

    bool isRunning = true;
    while (isRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        rotationAngle += 2.0;
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis);

        // Calcular la matriz de modelo
        uniforms.model = translation * rotation * scale;

        // Crear la matriz de vista usando el objeto cámara
        uniforms.view = glm::lookAt(
            camera.cameraPosition, // The position of the camera
            camera.targetPosition, // The point the camera is looking at
            camera.upVector        // The up vector defining the camera's orientation
        );

        framebuffer.clear();

        render(Primitive::TRIANGLES);

        framebuffer.renderBuffer(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(1);
    }

    return 0;
}