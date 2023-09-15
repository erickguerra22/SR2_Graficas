#pragma once
#include <glm/glm.hpp>
#include "uniforms.h"
#include "fragment.h"
#include <SDL.h>

Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // Aplicar las transformaciones al vértice utilizando las matrices de uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

    // Perspectiva
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Aplicar transformación del viewport
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);
    
    // Transformar la normal
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
    transformedNormal = glm::normalize(transformedNormal);

    return Vertex{
        glm::vec3(screenVertex),
        transformedNormal
    };
}

Fragment fragmentShader(Fragment& fragment) {

    fragment.color = fragment.color * fragment.intensity;

    return fragment;
}