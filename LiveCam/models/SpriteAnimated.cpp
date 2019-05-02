#include "SpriteAnimated.h"

SpriteAnimated::SpriteAnimated()
{
	animationOffset = { 0, 0, 0 };
	animationScale = { 1, 1 };
}

void SpriteAnimated::load()
{
	object = ModelLoader::CreateQuadModel();

	auto loadingResult = resourceManager.loadAnimation(animationPath);
	animationFramesIDs = loadingResult.IDs;
	animationSize = { loadingResult.animationWidth, loadingResult.animationHeight };
	animationLength = loadingResult.animationLength;

	animationIndex = 0;

	Matrix4d modelMatrixD;
	modelMatrixD = Affine3d(Translation3d(Vector3d((double)animationOffset[0], (double)animationOffset[1], (double)animationOffset[2]))).matrix();

	double Xscale = animationScale[0] / 2.f;
	double Yscale = -animationScale[1] / 2.f * animationSize[1] / animationSize[0];

	modelMatrixD *= Affine3d(Scaling(Xscale, Yscale, 1.)).matrix();

	modelMatrix = modelMatrixD.cast<float>();
}

void SpriteAnimated::animate()
{
	if (++animationIndex == animationLength) animationIndex = -1;
}

void SpriteAnimated::draw(Eigen::Matrix4f &externalModelMatrix, Eigen::Matrix4f &externalProjectionMatrix)
{
	if (animationIndex == -1)
	{
		return;
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	shader->begin();

	glEnable(GL_BLEND);

	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, animationFramesIDs[animationIndex]);

	Matrix4f resultModelMatrix = externalModelMatrix * modelMatrix;

	shader->setUniformMatrix4fv("ModelViewMatrix", 1, false, resultModelMatrix.data());
	shader->setUniformMatrix4fv("ProjectionMatrix", 1, false, externalProjectionMatrix.data());

	GLint uniformLocation = shader->GetUniformLocation("Texture");

	if (uniformLocation != -1)
	{
		glUniform1i(uniformLocation, 0);
	}

	VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, object.vb));
	VertexAttribSetter vTexCoord(VertexAttribSetter::TexCoordAttribSetter(*shader, object.tb));

	glDrawArrays(GL_TRIANGLES, 0, 6);

	shader->end();

	glPopAttrib();
}

void SpriteAnimated::unload()
{
	if (object.vb != 0)
	{
		glDeleteBuffers(1, &object.vb);
	}

	if (object.nb != 0)
	{
		glDeleteBuffers(1, &object.nb);
	}

	if (object.cb != 0)
	{
		glDeleteBuffers(1, &object.cb);
	}

	if (object.tb != 0)
	{
		glDeleteBuffers(1, &object.tb);
	}

	if (object.tangentb != 0)
	{
		glDeleteBuffers(1, &object.tangentb);
	}

	if (object.bitangentb != 0)
	{
		glDeleteBuffers(1, &object.bitangentb);
	}
}