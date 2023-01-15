#pragma once

#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "OpenGL\\libs\\OpenGL32.lib")
#pragma comment(lib, "OpenGL\\libs\\glut.lib")
#pragma comment(lib, "OpenGL\\libs\\glut32.lib")
#pragma comment(lib, "OpenGL\\libs\\glew32")
#include <Windows.h>
#include "OpenGL\include\GL.h"
#include <vector>
#include <stdio.h>
#include "OpenGL\glm\glm\glm.hpp"
using namespace std;

class ObjReader {
	vector < glm::vec3 > out_vertices;
	vector < glm::vec2 > out_uvs;
	vector < glm::vec3 > out_normals;

public:
	ObjReader() {}
	~ObjReader() {}

	bool load(const char* filename, GLuint& vertexList, float scale = 200) {
		vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		vector< glm::vec3 > temp_vertices;
		vector< glm::vec2 > temp_uvs;
		vector< glm::vec3 > temp_normals;

		FILE * file = fopen(filename, "r");
		if (file == NULL) {
			fprintf(stderr, "Impossible to open the file !\n");
			return false;
		}

		out_vertices.clear();
		out_uvs.clear();
		out_normals.clear();

		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

					   // else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);

			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);

			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					fprintf(stderr, "Unsuported format\n");
					return false;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

			}

		}
		fclose(file);

		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			unsigned int vertexIndex = vertexIndices[i];
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			out_vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			unsigned int UV_Index = uvIndices[i];
			glm::vec2 UV = temp_uvs[UV_Index - 1];
			out_uvs.push_back(UV);
		}

		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			unsigned int normalIndex = normalIndices[i];
			glm::vec3 normal = temp_normals[normalIndex - 1];
			out_normals.push_back(normal);
		}

		// Create vertex list
		vertexList = glGenLists(1);

		glPointSize(2.0);
		glNewList(vertexList, GL_COMPILE);
		{
			glPushMatrix();
			glBegin(GL_TRIANGLES);

			for (size_t i = 0; i < out_vertices.size(); ++i) {
				glm::vec3 v = out_vertices[i];
				glVertex3f(v.x * scale, v.y * scale, v.z * scale);
			}

			glEnd();
		}
		glPopMatrix();
		glEndList();

		return true;
	}
};