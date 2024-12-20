#pragma once
#include "Shader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
enum class TEXTURETYPE
{
	DIFFUSE = 0, SPECULAR, NORMAL, AMBIENT
};

struct Vertices
{
	glm::vec3 aPos;
	glm::vec3 aNormal;
	glm::vec3 aTangent;
	glm::vec2 aTexCoord;
};

struct Material
{
	uint32_t texID;
	TEXTURETYPE texType;
	std::string path;
	float Ns;
};

class Mesh
{
public:
	Mesh(std::vector<Vertices> vertices, std::vector<Material> materials, std::vector<unsigned int> indices)
		: m_Vertices(vertices), m_Materials(materials), m_Indices(indices) {
		setUp();
	}
	void Draw(Shader& shader);
private:
	std::vector<Vertices> m_Vertices;
	std::vector<Material> m_Materials;
	std::vector<unsigned int> m_Indices;
	uint32_t m_MeshVao;
private:
	void setUp();
};

class Model
{
public:
	Model(const std::string& path);
	void Draw(Shader& shader);
private:
	std::vector<Mesh> m_Meshes;
	std::vector<Material> m_Texs;
	std::string m_ObjDir;
private:
	void processNode(const aiScene *scene, const aiNode* node);
	Mesh processMesh(const aiScene* scene, const aiMesh* mesh);
	std::vector<Material> processMaterial(const aiScene* scene, const aiMaterial* material, const aiTextureType& type, const TEXTURETYPE& m_type);
};

