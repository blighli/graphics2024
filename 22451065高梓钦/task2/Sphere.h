#pragma once

#include "glm/glm.hpp"
#include <cmath>
#include <vector>

using namespace std;

class Sphere
{
private:
	int _numVertices;   //顶点总数
	int _numIndices;    //顶点索引总数

	vector<int> _indices;
	vector<glm::vec3> _vertices;  //顶点向量数
	vector<glm::vec2> _texCoords;
	vector<glm::vec3> _normals;
	vector<glm::vec3> _tangents;  //切片数量

	void init(int prec);
	float toRadians(float degree);

public:
	Sphere();
	Sphere(int prec);

	int getNumVertices();
	int getNumIndices();

	vector<int> getIndices();
	vector<glm::vec3> getVertices();
	vector<glm::vec2> getTexCoords();
	vector<glm::vec3> getNormals();
	vector<glm::vec3> getTangents();

};


