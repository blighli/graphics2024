// Model.h
#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "tiny_obj_loader.h"
#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Model {
public:
    Model(const std::string& path);
    void Draw(Shader& shader);

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    void loadModel(const std::string& path);
    void setupMesh();
};

#endif
