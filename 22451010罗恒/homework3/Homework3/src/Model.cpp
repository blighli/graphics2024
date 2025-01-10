#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

static uint32_t getTexId(const std::string& texPath, bool use_s = true);
void Mesh::Draw(Shader& shader)
{
	shader.use();
	uint32_t diffuse_count = 0, specular_count = 0, normal_count = 0, ambient_count = 0;
	for (uint32_t i = 0; i < m_Materials.size(); i++)
	{
		std::string name = "";
		switch (m_Materials[i].texType)
		{
		case TEXTURETYPE::DIFFUSE:
			name = "uTextures.diffuse" + std::to_string(diffuse_count++);
			break;
		case TEXTURETYPE::NORMAL:
			name = "uTextures.normal" + std::to_string(normal_count++);
			break;
		case TEXTURETYPE::SPECULAR:
			name = "uTextures.specular" + std::to_string(specular_count++);
			shader.setValue("uTextures.ns", m_Materials[i].Ns);
			break;
		case TEXTURETYPE::AMBIENT:
			name = "uTextures.ambient" + std::to_string(ambient_count++);
			break;
		}
		
		shader.setValue(name, i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_Materials[i].texID);
	}
	glBindVertexArray(m_MeshVao);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setUp()
{
	uint32_t vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertices), &m_Vertices[0], GL_STATIC_DRAW);

	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)(offsetof(Vertices, aNormal)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)(offsetof(Vertices, aTangent)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)(offsetof(Vertices, aTexCoord)));
	}

	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * (sizeof(uint32_t)), &m_Indices[0], GL_STATIC_DRAW);
	}
	m_MeshVao = vao;
	glBindVertexArray(0);
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : m_Meshes)
	{
		mesh.Draw(shader);
	}
}

Model::Model(const std::string& path)
{
	m_ObjDir = path.substr(0, path.find_last_of('/'));
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (scene == NULL)
	{
		std::cerr << "error to load object" << std::endl;
		return;
	}
	processNode(scene, scene->mRootNode);
}

void Model::processNode(const aiScene* scene, const aiNode* node)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		m_Meshes.push_back(processMesh(scene, scene->mMeshes[node->mMeshes[i]]));
	}
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(scene, node->mChildren[i]);
	}
}

Mesh Model::processMesh(const aiScene* scene, const aiMesh* mesh)
{
	std::vector<Vertices> vertices;
	std::vector<Material> materials;
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		aiVector3D ver = mesh->mVertices[i];
		aiVector3D normal = mesh->mNormals[i];
		aiVector3D texCoord = mesh->mTextureCoords[0][i];
		aiVector3D tangent = mesh->mTangents[i];
		vertices.push_back({
			glm::vec3(ver.x, ver.y, ver.z),
			glm::vec3(normal.x, normal.y, normal.z),
			glm::vec3(tangent.x, tangent.y, tangent.z),
			glm::vec2(texCoord.x, texCoord.y)
		});
	}

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Material> diffuseTexs = processMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, TEXTURETYPE::DIFFUSE);
	materials.insert(materials.end(), diffuseTexs.begin(), diffuseTexs.end());
	std::vector<Material> ambientTexs = processMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_AMBIENT, TEXTURETYPE::AMBIENT);
	materials.insert(materials.end(), ambientTexs.begin(), ambientTexs.end());
	std::vector<Material> normalTexs = processMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_HEIGHT, TEXTURETYPE::NORMAL);
	materials.insert(materials.end(), normalTexs.begin(), normalTexs.end());
	std::vector<Material> specularTexs = processMaterial(scene, scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, TEXTURETYPE::SPECULAR);
	materials.insert(materials.end(), specularTexs.begin(), specularTexs.end());
	return Mesh(vertices, materials, indices);
}

std::vector<Material> Model::processMaterial(const aiScene* scene, const aiMaterial* material, const aiTextureType& type, const TEXTURETYPE& m_type)
{
	std::vector<Material> res;

	for (int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString aistring;
		material->GetTexture(type, i, &aistring);
		bool skip = false;
		std::string PicPath = m_ObjDir + "/" + aistring.C_Str();
		for (int i = 0; i < m_Texs.size(); i++)
		{
			if (PicPath == m_Texs[i].path)
			{
				res.push_back(m_Texs[i]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			float ns;
			material->Get(AI_MATKEY_SHININESS, ns);
			Material tex;
			tex.path = PicPath;
			tex.texType = m_type;
			bool use_s = true;
			if (m_type == TEXTURETYPE::NORMAL) use_s = false;
			tex.texID = getTexId(PicPath, use_s);
			tex.Ns = ns;
			res.push_back(tex);
			m_Texs.push_back(tex);
		}
	}
	return res;
}

static uint32_t getTexId(const std::string& texPath, bool use_s)
{
	static uint32_t m_RendererID;
	int width, height, channel;
	GLenum dataFormat, internalFormat;
	
	stbi_uc* data = stbi_load(texPath.c_str(), &width, &height, &channel, 0);
	if (!data)
	{
		std::cout << "model error load pic: \t" + texPath << std::endl;
		return 0;
	}
	switch (channel)
	{
	case 1:
		dataFormat = GL_RED;
		internalFormat = GL_RED;
		break;
	case 3:
		dataFormat = GL_RGB;
		internalFormat = GL_RGB;
		if (use_s)
		{
			internalFormat = GL_SRGB;
		}
		break;
	case 4:
		dataFormat = internalFormat = GL_RGBA;
		if (use_s)
		{
			internalFormat = GL_SRGB_ALPHA;
		}
		break;
	}
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glGenerateMipmap(GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	return m_RendererID;
}
