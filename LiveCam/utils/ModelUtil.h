#ifndef MODELUTIL_H
#define MODELUTIL_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "opencv2/core/core.hpp"

#include <models\DrawObjectsModel.h>

#pragma once

using namespace std;

class ModelUtil {
public:
	ModelUtil();
	~ModelUtil();
	
	static void checkErrors(std::string desc) {
		GLenum e = glGetError();
		if (e != GL_NO_ERROR) {
			fprintf(stderr, "OpenGL error in \"%s\": %d (%d)\n", desc.c_str(), e, e);
			exit(20);
		}
	}

	static void addGlTexture(cv::Mat image){

		char * buffer = new char[image.rows*image.cols*image.channels()];
		int step = image.step;
		int height = image.rows;
		int width = image.cols;
		int channels = image.channels();
		char * data = (char *)image.data;

		for (int i = 0; i < height; i++)
		{
			memcpy(&buffer[i*width*channels], &(data[i*step]), width*channels);
		}
		if (channels == 4)
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				image.cols,
				image.rows,
				0,
				GL_BGRA_EXT,
				GL_UNSIGNED_BYTE,
				buffer);
		}
		else
		{
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGB,
				image.cols,
				image.rows,
				0,
				GL_BGR_EXT,
				GL_UNSIGNED_BYTE,
				buffer);
		}
		

		delete[] buffer;
	}


	
	static void drawObj(const std::vector<DrawObjectsModel>& drawObjects) {

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		GLsizei stride = (3 + 3 + 3 + 2) * sizeof(float);

		for (size_t i = 0; i < drawObjects.size(); i++) {
			DrawObjectsModel o = drawObjects[i];
			if (o.vb < 1) {
				continue;
			}

			glBindBuffer(GL_ARRAY_BUFFER, o.vb);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);


			// currently not applying materials or textures in this manner, but we can in the future for preformance reasons...
			/*if ((o.material_id < materials.size())) {
			std::string diffuse_texname = materials[o.material_id].diffuse_texname;
			if (textures.find(diffuse_texname) != textures.end()) {
			glBindTexture(GL_TEXTURE_2D, textures[diffuse_texname]);
			}
			}*/

			glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
			glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
			glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
			glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
			//continue;
			// CheckErrors("drawarrays");
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// currently not drawing wires...
		return;
		
		// draw wireframe
		glDisable(GL_POLYGON_OFFSET_FILL);

		glPolygonMode(GL_FRONT, GL_LINE);

		glPolygonMode(GL_BACK, GL_LINE);
		//return;
		glColor3f(0.0f, 0.0f, 0.0f);
		for (size_t i = 0; i < drawObjects.size(); i++) {
			DrawObjectsModel o = drawObjects[i];
			if (o.vb < 1) {
				continue;
			}

			glBindBuffer(GL_ARRAY_BUFFER, o.vb);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glVertexPointer(3, GL_FLOAT, stride, (const void*)0);
			glNormalPointer(GL_FLOAT, stride, (const void*)(sizeof(float) * 3));
			glColorPointer(3, GL_FLOAT, stride, (const void*)(sizeof(float) * 6));
			glTexCoordPointer(2, GL_FLOAT, stride, (const void*)(sizeof(float) * 9));

			// drawing vertices
			glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
			checkErrors("drawarrays");
		}
	}
};
#endif