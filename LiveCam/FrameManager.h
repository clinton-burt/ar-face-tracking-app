#ifndef FRAME_MANAGER_H_INCLUDED
#define FRAME_MANAGER_H_INCLUDED


#include <iostream>
#include <vector>
#include <array>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>


struct FrameRenderBuffer
{
	GLuint frameBuffer = 0;
	GLuint depthRenderBuffer = 0;
	GLuint textureId = 0;
	GLuint add_textureId = 0;
	GLuint alphaMaskId = 0;
	GLuint filterMaskId = 0;
	size_t width = 0;
	size_t height = 0;
};

enum class ColorBuffer
{
	COLOR_1 = GL_COLOR_ATTACHMENT0, // main color buffer
	COLOR_2 = GL_COLOR_ATTACHMENT1, // auxiliary buffer
	FILTER_MASK = GL_COLOR_ATTACHMENT2,
	ALPHA_MASK = GL_COLOR_ATTACHMENT3
};

class FrameManager
{

public:
	FrameManager();
	~FrameManager();

	size_t currentFBOindex;
	size_t currentBuffer;

	size_t AddFrameBuffer(size_t width, size_t height);

	void RemoveFrameBuffer(size_t frameIndex);

	void SwitchToFrameBuffer(size_t frameIndex);

	void SwitchToDrawBuffer(size_t bufferIndex);

	void SwitchToScreen();

	FrameRenderBuffer getFrameRenderBuffer(size_t frameIndex);

	size_t getCurrentFBOindex();
	size_t getCurrentDrawBuffer();

protected:
	static size_t nextFrameIndex;

	std::map<size_t, FrameRenderBuffer> frameBufferMap;

	void DeleteFrameBuffer(FrameRenderBuffer& frameBuffer);
};

#endif //FRAME_MANAGER_H_INCLUDED
