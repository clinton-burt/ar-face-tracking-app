#ifndef DRAWOBJECTSMODEL_H
#define DRAWOBJECTSMODEL_H

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

class DrawObjectsModel
{
public:

	int numTriangles;

	size_t material_id;
	std::string material;

	GLuint vb = 0;
	GLuint nb = 0;
	GLuint cb = 0;
	GLuint tb = 0;

	GLuint tangentb = 0;
	GLuint bitangentb = 0;

	std::string name;

// 	std::array<float, 3> minimums;
// 	std::array<float, 3> maximums;
	float minimumZ = 0;

};
#endif