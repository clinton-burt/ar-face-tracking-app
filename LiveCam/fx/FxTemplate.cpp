#include <fx/FxTemplate.h>

FxTemplate::FxTemplate(FilterUiModel* externalInfo)
{
	loadFromJSON(boost::filesystem::path(externalInfo->getJSONsource()), externalInfo);
}

FxTemplate::~FxTemplate()
{

}

void FxTemplate::load()
{
	FxCore::load();
}

void FxTemplate::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	FxCore::transform(face, externalRenderParams);

	for (auto model : models)
	{
		if (model->getTypeName() != OpenGL3DModel::TYPE_NAME)
		{
			continue;
		}

		auto model3D = (OpenGL3DModel*)model.get();

		if (!model3D->backClipping)
		{
			continue;
		}

		Eigen::Vector4f minimum = { model3D->bmin[0], model3D->bmin[1], model3D->bmin[2], 1 };
		Eigen::Vector4f maximum = { model3D->bmax[0], model3D->bmax[1], model3D->bmax[2], 1 };

		Eigen::Matrix4f headAdjusting = model3D->renderParams.additionalMatrices4[0];

		auto zRotationData = model3D->renderParams.rotationMatrix.block<1, 3>(2, 0).data();
		Eigen::Vector3f zRotation = { zRotationData[0], zRotationData[1], zRotationData[2] };

		minimum = headAdjusting * minimum;
		maximum = headAdjusting * maximum;

		float Yclip = minimum[1] + (maximum[1] - minimum[1]) * model3D->Yclip;
		float Zclip = minimum[2] + (maximum[2] - minimum[2]) * model3D->Zclip;

		auto params = model3D->objectRenderParams[face.pointId].begin();
		for (auto &obj : model3D->objects)
		{
			(*params)->additionalUniforms["YClip"] = TUniform1f(Yclip);
			(*params)->additionalUniforms["ZClip"] = TUniform1f(Zclip);
			(*params)->additionalUniforms["ZRotation"] = TUniform3f(zRotation);
			(*params)->additionalUniforms["HeadAdjustingMatrix"] = TUniform16(headAdjusting);
			++params;
		}
	}
}