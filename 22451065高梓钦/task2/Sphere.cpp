#include "Sphere.h"

static const float pai = 3.1415926f;

Sphere::Sphere()
{
	init(48);
}

Sphere::Sphere(int prec)
{
	init(prec);
}

void Sphere::init(int prec)  //prec:表示精度，即一个球体被切成prec片
{
	_numVertices = (prec + 1) * (prec + 1);   //顶点总数
	_numIndices = prec * prec * 6;  //每个顶点周围被6个三角形包围，索引总数为：prec * prec * 6

	for (int i = 0; i < _numVertices; i++)
	{
		_vertices.push_back(glm::vec3());
		_texCoords.push_back(glm::vec2());
		_normals.push_back(glm::vec3());
		_tangents.push_back(glm::vec3());
	}

	for (int i = 0; i < _numIndices; i++)
	{
		_indices.push_back(0);
	}

	// calculate triangle vertices
	for (int i = 0; i <= prec; i++)
	{
		for (int j = 0; j <= prec; j++)
		{
			float y = (float)(glm::cos(toRadians(180.f - i * 180.f / prec)));
			float x = (float)(glm::cos(toRadians(j * 360.f / prec)) * ((float)(glm::abs(glm::cos(glm::asin(y))))));
			float z = (float)(glm::sin(toRadians(j * 360.f / (float)(prec))) * (float)(glm::abs(glm::cos(glm::asin(y)))));
			_vertices[i * (prec + 1) + j] = glm::vec3(x, y, z);
			_texCoords[i * (prec + 1) + j] = glm::vec2(((float)(j) / prec), ((float)(i) / prec));
			_normals[i * (prec + 1) + j] = glm::vec3(x, y, z);

			// calculate tangent vector
			if (((0 == x) && (1 == y) && (0 == z)) || ((0 == x) && (-1 == y) && (0 == z)))
			{
				_tangents[i * (prec + 1) + j] = glm::vec3(0.f, 0.f, -1.f);
			}
			else
			{
				_tangents[i * (prec + 1) + j] = glm::cross(glm::vec3(0.f, 1.f, 0.f), glm::vec3(x, y, z));
			}
		}
	}

	// calculate triangle indices
	for (int i = 0; i < prec; i++)
	{
		for (int j = 0; j < prec; j++)
		{
			_indices[6 * (i * prec + j) + 0] = i * (prec + 1) + j;
			_indices[6 * (i * prec + j) + 1] = i * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 2] = (i + 1) * (prec + 1) + j;
			_indices[6 * (i * prec + j) + 3] = i * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 4] = (i + 1) * (prec + 1) + j + 1;
			_indices[6 * (i * prec + j) + 5] = (i + 1) * (prec + 1) + j;
		}
	}
}

float Sphere::toRadians(float degree)
{
	return (degree * 2.f * pai) / 360.f;
}



int Sphere::getNumVertices()
{
	return _numVertices;
}

int Sphere::getNumIndices()
{
	return _numIndices;
}

std::vector<int> Sphere::getIndices()
{
	return _indices;
}

std::vector<glm::vec3> Sphere::getVertices()
{
	return _vertices;
}

std::vector<glm::vec2> Sphere::getTexCoords()
{
	return _texCoords;
}

std::vector<glm::vec3> Sphere::getNormals()
{
	return _normals;
}

std::vector<glm::vec3> Sphere::getTangents()
{
	return _tangents;
}

