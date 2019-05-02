#include "FrameManager.h"

#include "Resolutions.h"

size_t FrameManager::nextFrameIndex = 1;

FrameManager::FrameManager()
{
	currentFBOindex = 0;
	currentBuffer = 0;
}


FrameManager::~FrameManager()
{
	for (auto itr = frameBufferMap.begin(); itr != frameBufferMap.end(); itr++)
	{
		DeleteFrameBuffer(itr->second);
	}

	frameBufferMap.clear();


}

size_t FrameManager::getCurrentFBOindex()
{
	return currentFBOindex;
}
size_t FrameManager::getCurrentDrawBuffer()
{
	return currentBuffer;;
}

size_t FrameManager::AddFrameBuffer(size_t width, size_t height)
{
	
	FrameRenderBuffer frameRenderBuffer;

	glGenTextures(1, &frameRenderBuffer.textureId);
	glBindTexture(GL_TEXTURE_2D, frameRenderBuffer.textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &frameRenderBuffer.add_textureId);
	glBindTexture(GL_TEXTURE_2D, frameRenderBuffer.add_textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &frameRenderBuffer.alphaMaskId);
	glBindTexture(GL_TEXTURE_2D, frameRenderBuffer.alphaMaskId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &frameRenderBuffer.filterMaskId);
	glBindTexture(GL_TEXTURE_2D, frameRenderBuffer.filterMaskId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	GLuint status;

	glGenFramebuffers(1, &frameRenderBuffer.frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameRenderBuffer.frameBuffer);

	glGenRenderbuffers(1, &frameRenderBuffer.depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, frameRenderBuffer.depthRenderBuffer);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameRenderBuffer.depthRenderBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)ColorBuffer::COLOR_1, GL_TEXTURE_2D, frameRenderBuffer.textureId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)ColorBuffer::COLOR_2, GL_TEXTURE_2D, frameRenderBuffer.add_textureId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)ColorBuffer::FILTER_MASK, GL_TEXTURE_2D, frameRenderBuffer.filterMaskId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)ColorBuffer::ALPHA_MASK, GL_TEXTURE_2D, frameRenderBuffer.alphaMaskId, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:

		currentFBOindex = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, currentFBOindex); //This will not work in iOS !!! -- Vladislav Khorev vladislav.khorev@fishrungames.com
		currentBuffer = GL_COLOR_ATTACHMENT0;
		glDrawBuffer(currentBuffer);

		break;
	default:

		glDeleteTextures(1, &frameRenderBuffer.textureId);
		glDeleteTextures(1, &frameRenderBuffer.add_textureId);
		glDeleteTextures(1, &frameRenderBuffer.alphaMaskId);
		glDeleteTextures(1, &frameRenderBuffer.filterMaskId);

		glDeleteRenderbuffers(1, &frameRenderBuffer.depthRenderBuffer);
		currentFBOindex = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, currentFBOindex); //This will not work in iOS !!! -- Vladislav Khorev vladislav.khorev@fishrungames.com
		currentBuffer = GL_COLOR_ATTACHMENT0;
		glDrawBuffer(currentBuffer);
		glDeleteFramebuffers(1, &frameRenderBuffer.frameBuffer);

		std::cerr << "Failed to create frame buffer object " << std::endl;
		return 0;

	}

	frameRenderBuffer.width = width;
	frameRenderBuffer.height = height;

	size_t result = nextFrameIndex;

	frameBufferMap[result] = frameRenderBuffer;

	nextFrameIndex++;

	return result;

	
}

void FrameManager::RemoveFrameBuffer(size_t frameIndex)
{

	if (frameBufferMap.find(frameIndex) == frameBufferMap.end())
	{
		std::cerr << "FrameManager - Trying to remove not existing Frame Buffer" << std::endl;
		return;
	}

	DeleteFrameBuffer(frameBufferMap[frameIndex]);

	frameBufferMap.erase(frameIndex);


}

void FrameManager::SwitchToFrameBuffer(size_t frameIndex)
{
	if (frameBufferMap.find(frameIndex) == frameBufferMap.end())
	{
		std::cerr << "FrameManager - Trying to switch to not existing Frame Buffer" << std::endl;
		return;
	}
	
	currentFBOindex = frameBufferMap[frameIndex].frameBuffer;
	glBindFramebuffer(GL_FRAMEBUFFER, currentFBOindex);
	currentBuffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffer(currentBuffer);
	glViewport(0, 0, frameBufferMap[frameIndex].width, frameBufferMap[frameIndex].height);
}

void FrameManager::SwitchToDrawBuffer(size_t bufferIndex)
{
	currentBuffer = bufferIndex;
	glDrawBuffer(currentBuffer);
}

void FrameManager::SwitchToScreen()
{
	currentFBOindex = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, currentFBOindex);
	currentBuffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffer(currentBuffer);
	glViewport(0, 0, Resolutions::OUTPUT_WIDTH, Resolutions::OUTPUT_HEIGHT);
}


FrameRenderBuffer FrameManager::getFrameRenderBuffer(size_t frameIndex)
{
	if (frameBufferMap.find(frameIndex) == frameBufferMap.end())
	{
		std::cerr << "FrameManager - Trying to get not existing Frame Buffer" << std::endl;
		return FrameRenderBuffer();
	}

	return frameBufferMap[frameIndex];
}

void FrameManager::DeleteFrameBuffer(FrameRenderBuffer& frameBufferRecord)
{
	glDeleteTextures(1, &frameBufferRecord.textureId);
	glDeleteTextures(1, &frameBufferRecord.add_textureId);
	glDeleteTextures(1, &frameBufferRecord.alphaMaskId);
	glDeleteTextures(1, &frameBufferRecord.filterMaskId);

	glDeleteRenderbuffers(1, &frameBufferRecord.depthRenderBuffer);
	currentFBOindex = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, currentFBOindex); //This will not work in iOS !!! -- Vladislav Khorev vladislav.khorev@fishrungames.com
	currentBuffer = GL_COLOR_ATTACHMENT0;
	glDrawBuffer(currentBuffer);
	glDeleteFramebuffers(1, &frameBufferRecord.frameBuffer);

	frameBufferRecord.depthRenderBuffer = 0;
	frameBufferRecord.frameBuffer = 0;
	frameBufferRecord.textureId = 0;
	frameBufferRecord.add_textureId = 0;
	frameBufferRecord.alphaMaskId = 0;
	frameBufferRecord.filterMaskId = 0;
	frameBufferRecord.width = 0;
	frameBufferRecord.height = 0;

}