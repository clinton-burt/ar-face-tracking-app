#include <fx/Fx3dModelAncestor.h>

#include <QDebug>

const std::string OpenGL3DModel::TYPE_NAME = "3DModel";

std::string OpenGL3DModel::getTypeName()
{
	return TYPE_NAME;
}

void OpenGL3DModel::onInputFrameResized()
{
	OpenGLModel::onInputFrameResized();

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		pivotX[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		pivotY[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
	}
}

OpenGL3DModel::OpenGL3DModel()
{
	bmin = { 0, 0, 0 };
	bmax = { 0, 0, 0 };

	backClipping = false;

	pivotOffset = { 0, 0, 0 };
}

boost::property_tree::ptree OpenGL3DModel::getPTree(ExtraModelData &data)
{
	auto tree = OpenGLModel::getPTree(data);

	tree.put("type", getTypeName());

	if (!modelPath.empty())
	{
		auto originFile = fs::path(data.root->resourcesRoot / modelPath);
		auto copiedFile = fs::path(data.root->filterFolder / fs::path(modelPath).filename());
		if (!equivalent(originFile, copiedFile))
		{
			fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
		}

		modelPath = fs::path(modelPath).filename().string();
		tree.put("OBJ", modelPath);
	}

	if (!modelList.empty())
	{
		boost::property_tree::ptree OBJs;

		for (auto &file : modelList)
		{

			auto originFile = fs::path(data.root->resourcesRoot / file);
			auto copiedFile = fs::path(data.root->filterFolder / fs::path(file).filename());
			if (!equivalent(originFile, copiedFile))
			{
				fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
			}

			file = fs::path(file).filename().string();

			boost::property_tree::ptree fileRecord;
			fileRecord.put("", file);
			OBJs.push_back(std::make_pair("", fileRecord));
		}

		tree.put_child("OBJs", OBJs);
	}

	if (!materialPath.empty())
	{
		auto originFile = fs::path(data.root->resourcesRoot / materialPath);
		auto copiedFile = fs::path(data.root->filterFolder / fs::path(materialPath).filename());
		if (!equivalent(originFile, copiedFile))
		{
			fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
		}

		materialPath = fs::path(materialPath).filename().string();
		tree.put("MTL", materialPath);
	}

	if (!materialList.empty())
	{
		boost::property_tree::ptree MTLs;

		for (auto &file : materialList)
		{

			auto originFile = fs::path(data.root->resourcesRoot / file);
			auto copiedFile = fs::path(data.root->filterFolder / fs::path(file).filename());
			if (!equivalent(originFile, copiedFile))
			{
				fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
			}

			file = fs::path(file).filename().string();

			boost::property_tree::ptree fileRecord;
			fileRecord.put("", file);
			MTLs.push_back(std::make_pair("", fileRecord));
		}

		tree.put_child("MTLs", MTLs);
	}

	if (modelShift != Vector3f(0, 0, 0))
	{
		boost::property_tree::ptree shiftTree;

		for (int i = 0; i < 3; ++i)
		{
			boost::property_tree::ptree childTree;
			childTree.put("", modelShift[i]);

			shiftTree.push_back(std::make_pair("", childTree));
		}

		tree.put_child("modelShift", shiftTree);
	}

	if (modelScale != 1)
	{
		tree.put("modelScale", modelScale);
	}

	return tree;
}

void OpenGL3DModel::prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData)
{
	auto renderIDs = findMatchingRenderParamsIndexes(modelData.root->filter->commonRenderParams, modelData);

	suit.put_child("renderParamsIDs", renderIDs);

	boost::property_tree::ptree modelTexturesTree;
	auto &modelTextures = suit.get_child_optional("modelTextures");
	if (modelTextures)
	{
		for (auto &texture : modelTextures.get())
		{
			boost::filesystem::path path(texture.second.get_value<std::string>());

			boost::property_tree::ptree tree;
			tree.put("", path.filename().string());
			modelTexturesTree.push_back(std::make_pair("", tree));

			if (!path.empty())
			{
				auto originFile = fs::path(modelData.root->resourcesRoot / path);
				auto copiedFile = fs::path(modelData.root->filterFolder / path.filename());
				if (!equivalent(originFile, copiedFile))
				{
					fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
				}
			}
		}

		suit.put_child("modelTextures", modelTexturesTree);
	}
}


void OpenGL3DModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	OpenGLModel::loadFromJSON(modelRecord);

	backClipping = modelRecord.get<bool>("backClipping", false);
	Yclip = modelRecord.get<float>("Yclip", 0);
	Zclip = modelRecord.get<float>("Zclip", 0);

	auto extraShiftVector = modelRecord.get_child_optional("modelShift");
	modelShift = extraShiftVector ? JSONVectorReader::readVector3f(extraShiftVector.get()) : Vector3f(0, 0, 0);

	modelScale = modelRecord.get<float>("modelScale", 1);

	auto rotateMatrixRecord = modelRecord.get_child_optional("modelRotation");
	extraRotateMatrix = rotateMatrixRecord ? JSONVectorReader::readMatrix3f(rotateMatrixRecord.get()) : Matrix3f::Identity();

	modelPath = modelRecord.get<std::string>("OBJ", "");

	materialPath = modelRecord.get<std::string>("MTL", "");

	auto OBJs = modelRecord.get_child_optional("OBJs");
	if (OBJs)
	{
		for (auto &record : OBJs.get())
		{
			modelList.push_back(record.second.get_value<std::string>());
		}
	}

	auto MTLs = modelRecord.get_child_optional("MTLs");
	if (MTLs)
	{
		for (auto &record : MTLs.get())
		{
			materialList.push_back(record.second.get_value<std::string>());
		}
	}
}

bool OpenGL3DModel::reloadOBJ()
{
	objects.clear();

	float extraScale;
	Vector3f extraShift;

	switch (transformVersion)
	{
	case TransformVersion::V2:
		extraScale = 1;
		extraShift = Vector3f(0, 0, 0);
		break;

	case TransformVersion::V1:
		extraScale = modelScale;
		extraShift = modelShift;
		break;
	}

	std::vector<std::string> OBJs;

	if (modelList.empty())
	{
		OBJs.push_back((resourceManager.resourceRoot / modelPath).string());
	}
	else
	{
		for (auto &file : modelList)
		{
			OBJs.push_back((resourceManager.resourceRoot / file).string());
		}
	}

	ModelLoader::LoadObj(OBJs, objects, bmin, bmax, extraScale, extraShift, extraRotateMatrix);

	ModelLoader::loadMTLnames(OBJs, objects);
	
	return true;
}

void OpenGL3DModel::sortMeshesInZAscending()
{
	std::vector<float> objectsDepths;
	std::vector<int> indexes;

	int i = 0;
	for (auto &obj : objects)
	{
		objectsDepths.push_back(obj.minimumZ);
		indexes.push_back(i++);
	}

	std::sort(indexes.begin(), indexes.end(), [objectsDepths](const int A, const int B) -> bool
	{
		return objectsDepths[A] < objectsDepths[B];
	});

	std::vector<DrawObjectsModel> sortedObjects;

	for (int index : indexes)
	{
		sortedObjects.push_back(objects[index]);
	}

	objects = sortedObjects;
}

void OpenGL3DModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately)
{
	OpenGLModel::applySuit(suit, faceIndex, commonRenderParams, loadTexturesImmediately);

	if (!visible[faceIndex])
	{
		return;
	}

	auto IDs = suit.get_child_optional("renderParamsIDs");
	if (IDs)
	{
		objectRenderParams[faceIndex].clear();
		for (auto &ID : IDs.get())
		{
			objectRenderParams[faceIndex].push_back(commonRenderParams[ID.second.get_value<size_t>()]);
		}
	}

	auto modelTextures = suit.get_child_optional("modelTextures");
	if (modelTextures)
	{
		texturesPaths[faceIndex].clear();

		for (auto &texture : modelTextures.get())
		{
			texturesPaths[faceIndex].push_back(texture.second.get_value<std::string>());
		}

		texturesIDs[faceIndex].clear();

		if (loadTexturesImmediately)
		{
			for (auto &texture : texturesPaths[faceIndex])
			{
				texturesIDs[faceIndex].push_back(resourceManager.loadTexture(texture).ID);
			}
		
			for (auto &params : objectRenderParams[faceIndex])
			{
				if (!params->normalMap.empty())
				{
					params->normalMapID = resourceManager.loadTexture(params->normalMap).ID;
				}
			}
		}
	}
}

bool OpenGL3DModel::load()
{
	OpenGLModel::load();

// 	if (isOrgModel)
// 	{
// 	 	if (false == ModelLoader::LoadObjAndConvert(modelPath, objects, materials, textures, fxModels[i].bmin, fxModels[i].bmax))
// 	 	{
// 	 		return false;
// 	 	}
//		else return true;
// 	}

	if  (!reloadOBJ())
	{
		return false;
	}

	float width = bmax[0] - bmin[0];
	float height = bmax[1] - bmin[1];
	float depth = bmax[2] - bmin[2];

	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		for (auto &particle : particles[i])
		{
			particle.autoScale = width / particle.effect.getEffectWidth() * particle.scale;
			particle.autoGlobalGravity = particle.globalGravity * height;
			particle.autoCoords =
			{ bmin[0] + particle.coords[0] * width, bmin[1] + particle.coords[1] * height, bmin[2] + particle.coords[2] * depth };
			particle.effect.setCoords(particle.autoCoords.head<3>());
		}
	}

	for (int face = 0; face < FaceTracker::MAX_TO_TRACK; ++face)
	{
		texturesIDs[face].clear();
		
		for (auto &texture : texturesPaths[face])
		{
			texturesIDs[face].push_back(resourceManager.loadTexture(texture).ID);

			qDebug() << "OpenGL3DModel::load: " << texturesIDs[face];
		}

		for (auto &params : objectRenderParams[face])
		{
			if (!params->normalMap.empty())
			{
				params->normalMapID = resourceManager.loadTexture(params->normalMap).ID;
			}
		}
	}

	return true;
}

void OpenGL3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	//Then modelview matrix

	auto xFaceCenterRawSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
	auto yFaceCenterRawSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double tx = xFaceCenterRawSmooth / face.fWidth;
	double ty = yFaceCenterRawSmooth / face.fHeight;

	Matrix4d modelMatrix;
	Matrix3f anti_effectRotation;

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);
		depth = -distance;

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		Vector3d shift(xShift, yShift, -distance);
		modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

		Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(shift);
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift * xShift + yShift * yShift)) / distance);

		Matrix3d correction(AngleAxisd(rotateAngle, rotateVector)); // correct look direction

		//x
		Matrix3d rotation (AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
		//y
		rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0)).matrix();
		//z
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1)).matrix();

		modelMatrix *= Affine3d(correction * rotation).matrix(); // rotation

		anti_effectRotation = rotation.inverse().cast<float>();

		Matrix4d headAdjusting = Matrix4d::Identity();

		shift = { modelShift[0] * STD_FACE_WIDTH,  modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
		headAdjusting *= Affine3d(Translation3d(Vector3d(shift[0], shift[1], shift[2] - STD_HEAD_LENGTH / 2))).matrix();
		// head center shift && custom head-relative shift

		double scale = faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
		headAdjusting *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with faceWidth & head-relative scale

		Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
		headAdjusting *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

		modelMatrix *= headAdjusting;

		renderParams.additionalMatrices4[0] = headAdjusting.cast<float>();

		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}



	case TransformVersion::V1:
	{
		float h = 30;

		double planeH = 2.f * h * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, 0))).matrix();
		Vector3d rotateVector = Vector3d(0.0, 0.0, -h).cross(Vector3d(xShift, yShift, -h));
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift*xShift + yShift*yShift)) / h);
		Matrix3d correction(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
		//x
		Matrix3d rotation(AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0)));
		//y
		rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0)).matrix();
		//z
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1)).matrix();
		modelMatrix *= Affine3d(correction * rotation).matrix(); // rotation
		anti_effectRotation = rotation.inverse().cast<float>();
		const double faceWidthToDepth = 1.0;
		double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
		double faceDepth = faceWidthToDepth * faceWidth;
		// depth
		modelMatrix *= Affine3d(Translation3d(0, 0, -faceDepth)).matrix();
		const double scaleK = 0.08;
		double scale = faceWidth * scaleK;
		// scale
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix();
		modelMatrix = Affine3d(Translation3d(0, 0, -h)).matrix() * modelMatrix;
		depth = -faceDepth * scale - h;
		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}
	}

	for (auto &particle : particles[face.pointId])
	{
		ParticleUpdateParams params;

		params.outer_transformation = renderParams.modelView;
		params.effect_scaling = Affine3f(Scaling(particle.autoScale)).matrix();
		params.effect_rotation = anti_effectRotation;
		params.globalGravity = particle.autoGlobalGravity;
		params.enable_effect_rotation = particle.enable_rotation;
		params.enable_effect_gravity = particle.enable_effect_gravity;
		params.enable_global_gravity = particle.enable_global_gravity;

		Vector3f newCoords = particle.autoCoords.head<3>();
		if (!particle.enable_rotation)
		{
			newCoords = anti_effectRotation * newCoords;
			particle.effect.setCoords(newCoords);
		}

		newCoords = (params.outer_transformation * Vector4f(newCoords[0], newCoords[1], newCoords[2], 1)).head<3>();

		Vector4f globalSpeed;
		globalSpeed << newCoords - particle.lastCoords, 1;
		Vector3f localSpeed = (params.outer_transformation.inverse() * globalSpeed).head<3>();

		particle.effect.setSpeed({ 0, 0, 0 }); // (localSpeed / externalRenderParams.timeShift);

		//Vector3f xxx = particle.effect.getSpeed();
		//qDebug() << xxx[0] << " " << xxx[1] << " " << xxx[2];

		particle.lastCoords = newCoords;

		particle.effect.update(externalRenderParams.timeShift, params);
	}

	//Then rotate yaw matrix
	renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
}

void OpenGL3DModel::draw(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	for (size_t i = 0; i < objects.size(); i++)
	{
		DrawObjectsModel o = objects[i];
		if (o.vb < 1)
		{
			continue;
		}

		int index = i < objectRenderParams[face.pointId].size() ? i : objectRenderParams[face.pointId].size() - 1;
		ObjectRenderParams *renderParams = objectRenderParams[face.pointId][index].get();

		if (!renderParams->visible)
		{
			continue;
		}

		GLuint texId = i < texturesIDs[face.pointId].size() ? texturesIDs[face.pointId][i] :
			texturesIDs[face.pointId].empty() ? 0 : texturesIDs[face.pointId].back();

		if (animateTextures[face.pointId]) {

			if (animateTexturesSpeed[face.pointId]==0) {

				if (animateTexturesCount[face.pointId] < texturesIDs[face.pointId].size() - 1) {

					animateTexturesCount[face.pointId]++;
				}
				else {

					animateTexturesCount[face.pointId] = 0;
				}

			}

			animateTexturesSpeed[face.pointId]++;

			if (animateTexturesSpeed[face.pointId]==2) {
				animateTexturesSpeed[face.pointId] = 0;
			}

			texId = texturesIDs[face.pointId][animateTexturesCount[face.pointId]];
			//
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);

		auto shader = renderParams->shader;

		if (renderParams->normalMapID != 0)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, renderParams->normalMapID);
		}

		ShaderSetter shaderSetter(shader, *this);

		SetUniformsForObject(*shader, i, face.pointId);

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*shader, o.vb));
		VertexAttribSetter vNormal(VertexAttribSetter::NormalAttribSetter(*shader, o.nb));
		VertexAttribSetter vTexCoord(VertexAttribSetter::TexCoordAttribSetter(*shader, o.tb));

		glDrawArrays(GL_TRIANGLES, 0, 3 * o.numTriangles);
	}

	for (auto &particle : particles[face.pointId])
	{
		Matrix4f projection = renderParams.projection;
		particle.effect.draw(projection);
	}

	glPopAttrib();
}

void OpenGL3DModel::unload()
{
	OpenGLModel::unload();

	for (auto &IDs : texturesIDs)
	{
		IDs.clear();
	}
}

std::shared_ptr<ObjectRenderParams> OpenGL3DModel::createDefaultObjectRenderParams()
{
	auto result = std::make_shared<ObjectRenderParams>();

	result->shadersSources.first = "./assets/shaders/vertex/noTextureVertexShader.txt";
	result->shadersSources.second = "./assets/shaders/fragment/noTexturePhongFragmentShader.txt";

	auto shader3D = shaderManagerWrapper.LoadFromFile(result->shadersSources.first.data(), result->shadersSources.second.data());
	result->shader = shader3D;
	result->additionalUniforms["cameraPos"] = TUniform4f(0, 0, 0, 1);
	result->additionalUniforms["lightPos"] = TUniform4f(0.2f, 0.2f, 1, 0);
	result->additionalUniforms["materialColor"] = TUniform4f(1, 1, 1, 1);
	result->additionalUniforms["ambientLight"] = TUniform3f(1, 1, 1);
	result->additionalUniforms["diffuseLight"] = TUniform3f(0, 0, 0);
	result->additionalUniforms["specularLight"] = TUniform3f(0, 0, 0);
	result->additionalUniforms["specularPower"] = TUniform1f(1);
	result->additionalUniforms["reflectionRatio"] = TUniform1f(0);

	return result;
}

void OpenGL3DModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
	params.additionalUniforms["cameraPos"] = TUniform4f(0, 0, 0, 1);
	params.additionalUniforms["lightPos"] = TUniform4f(0.2f, 0.2f, 1, 0);
	params.additionalUniforms["materialColor"] = TUniform4f(1, 1, 1, 1);
	params.additionalUniforms["ambientLight"] = TUniform3f(0, 0, 0);
	params.additionalUniforms["diffuseLight"] = TUniform3f(0, 0, 0);
	params.additionalUniforms["specularLight"] = TUniform3f(0, 0, 0);
	params.additionalUniforms["specularPower"] = TUniform1f(1);
	params.additionalUniforms["reflectionRatio"] = TUniform1f(0);
}

//======================================================================================

const std::string LipsJoint3DModel::TYPE_NAME = "LipsJoint3DModel";

std::string LipsJoint3DModel::getTypeName()
{
	return TYPE_NAME;
}

LipsJoint3DModel::LipsJoint3DModel()
{
	pivotOffset = { 0, -0.06, 0 };
}

void LipsJoint3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	//Then modelview matrix

	auto xLipsCenter = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xTopLipCenter);
	auto yLipsCenter = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yTopLipCenter);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double tx = xLipsCenter / face.fWidth;
	double ty = yLipsCenter / face.fHeight;

	Matrix4d modelMatrix;

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		AngleAxisd rotationX(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		AngleAxisd rotationY(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		AngleAxisd rotationZ(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		Eigen::Vector3d rotatedPivot = rotationZ * rotationY * rotationX * pivotOffset;

		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE) - rotatedPivot[2];

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		Vector3d shift(xShift, yShift, -distance);
		modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

		Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(shift);
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift * xShift + yShift * yShift)) / distance);

		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
		rotation *= rotationX;
		rotation *= rotationY;
		rotation *= rotationZ;

		modelMatrix *= rotation.matrix(); // rotation

		shift = { modelShift[0] * STD_FACE_WIDTH, modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
		modelMatrix *= Affine3d(Translation3d(shift)).matrix(); // custom head-relative shift

		double scale = faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with faceWidth & head-relative scale

		Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
		modelMatrix *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

		depth = 0;

		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}



	case TransformVersion::V1:
	{
		float h = 30;

		
		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, 0))).matrix();
		Vector3d rotateVector = Vector3d(0.0, 0.0, -h).cross(Vector3d(xShift, yShift, -h));
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift*xShift + yShift*yShift)) / h);
		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector));
		//x
		rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		//y
		rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		//z
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
		modelMatrix *= rotation.matrix();
		const double faceWidthToDepth = 1.0;
		double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
		double faceDepth = faceWidthToDepth * faceWidth;
		// depth
		modelMatrix *= Affine3d(Translation3d(0, 0, -faceDepth)).matrix();
		const double scaleK = 0.08;
		double scale = faceWidth * scaleK;
		// scale
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix();
		modelMatrix = Affine3d(Translation3d(0, 0, -h)).matrix() * modelMatrix;
		depth = -faceDepth * scale - h;
		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}
	}

	//Then rotate yaw matrix
	renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
}


//======================================================================================
LipsBottomJoint3DModel::LipsBottomJoint3DModel()
{
	pivotOffset = { 0, -0.09, 0 };
}

void LipsBottomJoint3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	//Then modelview matrix

	auto xLipsCenter = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xBottomLipCenter);
	auto yLipsCenter = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yBottomLipCenter);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	double tx = xLipsCenter / face.fWidth;
	double ty = yLipsCenter / face.fHeight;

	Matrix4d modelMatrix;

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		AngleAxisd rotationX(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		AngleAxisd rotationY(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		AngleAxisd rotationZ(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		Eigen::Vector3d rotatedPivot = rotationZ * rotationY * rotationX * pivotOffset;

		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE) - rotatedPivot[2];

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		Vector3d shift(xShift, yShift, -distance);
		modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

		Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(shift);
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift * xShift + yShift * yShift)) / distance);

		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
		rotation *= rotationX;
		rotation *= rotationY;
		rotation *= rotationZ;

		modelMatrix *= rotation.matrix(); // rotation

		shift = { modelShift[0] * STD_FACE_WIDTH, modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
		modelMatrix *= Affine3d(Translation3d(shift)).matrix(); // custom head-relative shift

		double scale = faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with faceWidth & head-relative scale

		Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
		modelMatrix *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

		depth = 0;

		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}


	case TransformVersion::V1:
	{
		float h = 30;

		
		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE);

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (tx - 0.5) * planeW;
		double yShift = -(ty - 0.5) * planeH;

		modelMatrix = Affine3d(Translation3d(Vector3d(xShift, yShift, 0))).matrix();
		Vector3d rotateVector = Vector3d(0.0, 0.0, -h).cross(Vector3d(xShift, yShift, -h));
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xShift*xShift + yShift*yShift)) / h);
		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector));
		//x
		rotation *= AngleAxisd(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		//y
		rotation *= AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		//z
		rotation *= AngleAxisd(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));
		modelMatrix *= rotation.matrix();
		const double faceWidthToDepth = 1.0;
		double faceWidth = planeW * faceWidthRawSmooth / face.fWidth;
		double faceDepth = faceWidthToDepth * faceWidth;
		// depth
		modelMatrix *= Affine3d(Translation3d(0, 0, -faceDepth)).matrix();
		const double scaleK = 0.08;
		double scale = faceWidth * scaleK;
		// scale
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix();
		modelMatrix = Affine3d(Translation3d(0, 0, -h)).matrix() * modelMatrix;
		depth = -faceDepth * scale - h;
		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}
	}

	//Then rotate yaw matrix
	renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
}


//======================================================================================
LeftBrow3DModel::LeftBrow3DModel()
{
	pivotOffset = { -0.05, 0.05, 0 };

}

void LeftBrow3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	//Then modelview matrix

	auto xFaceCenterSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
	auto yFaceCenterSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

	auto xLeftBrow = pivotX[face.pointId].responsiveAnalogReadSimple(face.pts[38]);
	auto yLeftBrow = pivotY[face.pointId].responsiveAnalogReadSimple(face.pts[39]);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	Matrix4d modelMatrix;

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		AngleAxisd rotationX(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		AngleAxisd rotationY(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		AngleAxisd rotationZ(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		Eigen::Vector3d rotatedPivot = rotationZ * rotationY * rotationX * pivotOffset;

		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE) - rotatedPivot[2];

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (xLeftBrow / face.fWidth - 0.5) * planeW;
		double yShift = -(yLeftBrow / face.fHeight - 0.5) * planeH;

		Vector3d shift(xShift, yShift, -distance);
		modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

		double xPivotShift = (xFaceCenterSmooth / face.fWidth - 0.5) * planeW;
		double yPivotShift = -(yFaceCenterSmooth / face.fWidth - 0.5) * planeH;

		Vector3d pivotShift(xPivotShift, yPivotShift, -distance);
		Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(pivotShift);
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xPivotShift * xPivotShift + yPivotShift * yPivotShift)) / distance);

		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
		rotation *= rotationX;
		rotation *= rotationY;
		rotation *= rotationZ;

		modelMatrix *= rotation.matrix(); // rotation

		shift = { modelShift[0] * STD_FACE_WIDTH, modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
		modelMatrix *= Affine3d(Translation3d(shift)).matrix(); // custom head-relative shift

		Eigen::Vector2f browWidth = { face.pts[42] - face.pts[34] , 0 /*face.pts[43] - face.pts[35]*/ };
		double scale = /*browWidth.norm() / face.fWidth*/ faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with brow width & head-relative scale

		Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
		modelMatrix *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

		depth = 0;

		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}
	}

	//Then rotate yaw matrix
	renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
}

//======================================================================================
RightBrow3DModel::RightBrow3DModel()
{
	pivotOffset = { 0.05, 0.05, 0 };
}

void RightBrow3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	//Then modelview matrix

	auto xFaceCenterSmooth = renderParams.faceXCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.xFaceCenterRaw);
	auto yFaceCenterSmooth = renderParams.faceYCenterSmoother[face.pointId].responsiveAnalogReadSimple(face.yFaceCenterRaw);

	auto xLeftBrow = pivotX[face.pointId].responsiveAnalogReadSimple(face.pts[48]);
	auto yLeftBrow = pivotY[face.pointId].responsiveAnalogReadSimple(face.pts[49]);

	auto rollSmooth = renderParams.rollSmoother[face.pointId].responsiveAnalogReadSimple(face.roll);
	auto pitchSmooth = renderParams.pitchSmoother[face.pointId].responsiveAnalogReadSimple(face.pitch);
	auto yawSmooth = renderParams.yawSmoother[face.pointId].responsiveAnalogReadSimple(face.yaw);

	auto faceWidthRawSmooth = renderParams.faceWidthSmoother[face.pointId].responsiveAnalogReadSimple(face.faceWidthRaw);

	double faceWidth = faceWidthRawSmooth / face.fWidth;

	const double STD_FACE_WIDTH = 0.172;
	const double STD_FACE_DISTANCE = 0.6;
	const double STD_HEAD_LENGTH = 0.3;

	Matrix4d modelMatrix;

	switch (transformVersion)
	{
	case TransformVersion::V2:
	{
		AngleAxisd rotationX(pitchSmooth * M_PI / 180.0, Vector3d(1, 0, 0));
		AngleAxisd rotationY(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0));
		AngleAxisd rotationZ(rollSmooth * M_PI / 180.0, Vector3d(0, 0, 1));

		Eigen::Vector3d rotatedPivot = rotationZ * rotationY * rotationX * pivotOffset;

		double distance = STD_FACE_WIDTH / (faceWidth / STD_FACE_DISTANCE) - rotatedPivot[2];

		double planeH = 2.f * distance * tan(vAngle / 2);
		double planeW = planeH * aspect;

		double xShift = (xLeftBrow / face.fWidth - 0.5) * planeW;
		double yShift = -(yLeftBrow / face.fHeight - 0.5) * planeH;

		Vector3d shift(xShift, yShift, -distance);
		modelMatrix = Affine3d(Translation3d(shift)).matrix(); // translation

		double xPivotShift = (xFaceCenterSmooth / face.fWidth - 0.5) * planeW;
		double yPivotShift = -(yFaceCenterSmooth / face.fWidth - 0.5) * planeH;

		Vector3d pivotShift(xPivotShift, yPivotShift, -distance);
		Vector3d rotateVector = Vector3d(0.0, 0.0, -1).cross(pivotShift);
		rotateVector.normalize();
		double rotateAngle = atan((sqrt(xPivotShift * xPivotShift + yPivotShift * yPivotShift)) / distance);

		Affine3d rotation(AngleAxisd(rotateAngle, rotateVector)); // correct look direction
		rotation *= rotationX;
		rotation *= rotationY;
		rotation *= rotationZ;

		modelMatrix *= rotation.matrix(); // rotation

		shift = { modelShift[0] * STD_FACE_WIDTH, modelShift[1] * STD_FACE_WIDTH, modelShift[2] * STD_HEAD_LENGTH };
		modelMatrix *= Affine3d(Translation3d(shift)).matrix(); // custom head-relative shift

		Eigen::Vector2f browWidth = { face.pts[44] - face.pts[52] , 0 /*face.pts[43] - face.pts[35]*/ };
		double scale = /*browWidth.norm() / face.fWidth*/ faceWidth / (bmax[0] - bmin[0]) * distance / STD_FACE_DISTANCE * modelScale;
		modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix(); // match model width with brow width & head-relative scale

		Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
		modelMatrix *= Affine3d(Translation3d(autoShift)).matrix(); // autoshift

		depth = 0;

		renderParams.modelView = modelMatrix.cast<float>();
		renderParams.rotationMatrix = rotation.matrix().block<3, 3>(0, 0).cast<float>();
		break;
	}
	}

	//Then rotate yaw matrix
	renderParams.yawMatrix = Affine3d(AngleAxisd(yawSmooth * M_PI / 180.0, Vector3d(0, 1, 0))).matrix().block<3, 3>(0, 0).cast<float>();
}

Static3DModel::Static3DModel()
{
}

void Static3DModel::transform(FXModel& face, ExternalRenderParams &externalRenderParams)
{
	double vAngle = externalRenderParams.vAngle;

	double f = 1 / tan(vAngle / 2);

	double aspect = externalRenderParams.frameWidth / static_cast<double>(externalRenderParams.frameHeight);

	double zFar = externalRenderParams.zFar;
	double zNear = externalRenderParams.zNear;
	Matrix4f projection = Matrix4f::Zero();

	projection(0, 0) = f / aspect;
	projection(1, 1) = f;
	projection(2, 2) = (zFar + zNear) / (zNear - zFar);
	projection(3, 2) = -1;
	projection(2, 3) = 2 * zFar*zNear / (zNear - zFar);

	renderParams.projection = projection;

	Matrix4d modelMatrix;

	modelMatrix = Affine3d(Translation3d(Vector3d(modelShift[0], modelShift[1], modelShift[2]))).matrix(); // custom shift in meteres
	
	double scale = 1 / (bmax[0] - bmin[0]) * modelScale;
	modelMatrix *= Affine3d(Scaling(scale, scale, scale)).matrix(); // resize to custom meteres in X direction

	Vector3d autoShift = { -(bmax[0] + bmin[0]) / 2, -(bmax[1] + bmin[1]) / 2, -(bmax[2] + bmin[2]) / 2 };
	modelMatrix *= Affine3d(Translation3d(autoShift)).matrix(); // autocentering

	renderParams.modelView = modelMatrix.cast<float>();
	renderParams.rotationMatrix = Matrix3f::Identity();
	renderParams.yawMatrix = Matrix3f::Identity();
}