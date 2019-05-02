#include "OpenGLModel.h"

#include <unordered_map>
#include <chrono>

boost::format StructUniformFieldFormatter1("%s[%d].%s");
boost::format StructUniformFieldFormatter2("%s[%d].%s[%d].%s");

const std::vector<std::pair<std::string, std::pair<std::string, std::string>>> OpenGLModel::DefaultShadersMap =
{
	// don't change this elements order
	{ "regular", { "simpleVertexShader.txt", "phongFragmentShader.txt" } },
	{ "no texture", { "noTextureVertexShader.txt", "noTexturePhongFragmentShader.txt" } },
	//{ "normal map", { "simpleVertexShader.txt", "normalMapHalfSpacePhongFragmentShader.txt" } }
	{ "normal map", { "normalMapVertexShader.txt", "normalMapPhongFragmentShader.txt" } }
};

ObjectRenderParams::ObjectRenderParams()
	: cameraPos(0, 0, 0, 1)
	, lightPos(0, 0, 1, 0)
	, ambientLight(0.5, 0.5, 0.5)
	, diffuseLight(0.5, 0.5, 0.5)
	, specularLight(0, 0, 0)
	, specularPower(1)
	, materialColor(1, 1, 1, 1)
	, reflectionRatio(0)
	, shader(nullptr)
	, normalMapID(0)
	, depthTest(true)
	, depthMask(true)
	, alphaTest(false)
	, cullFace(true)
	, blend(false)
	, visible(true)
{

}

ObjectRenderParams::ObjectRenderParams(boost::property_tree::ptree& renderParams, fs::path& resourcesRoot)
{
	shadersSources = { renderParams.get<std::string>("vertexShader"), renderParams.get<std::string>("fragmentShader") };
	shadersSources.first = shadersSources.first;
	shadersSources.second = shadersSources.second;
	shader = shaderManagerWrapper.LoadFromFile((resourcesRoot / shadersSources.first).string().data(),
		(resourcesRoot / shadersSources.second).string().data());

	visible = renderParams.get<bool>("visible", true);
	depthTest = renderParams.get<bool>("depthTest", true);
	depthMask = renderParams.get<bool>("depthMask", true);
	alphaTest = renderParams.get<bool>("alphaTest", false);
	cullFace = renderParams.get<bool>("cullFace", true);
	blend = renderParams.get<bool>("blend", false);

	auto MTL = renderParams.get_child_optional("MTL"); // first read MTL, then can override some properties from MTL
	if (MTL)
	{
		applyMTL(MTL.get());
	}

	normalMap = renderParams.get<std::string>("normalMap", "");
	normalMapID = 0;

	additionalUniforms.clear();

	auto uniforms = renderParams.get_child_optional("uniforms");
	if (uniforms)
	{
		for (auto &uniform : uniforms.get())
		{
			auto record = JSONVectorReader::readUniform(uniform.second);
			additionalUniforms[record.first] = record.second;
		}
	}
}

void ObjectRenderParams::reset()
{
	cameraPos << 0, 0, 0, 1;
	lightPos << 0, 0, 1, 0;
	ambientLight << 0.5, 0.5, 0.5;
	diffuseLight << 0.5, 0.5, 0.5;
	specularLight << 0, 0, 0;
	specularPower = 1;
	materialColor << 1, 1, 1, 1;
	reflectionRatio = 0;
	normalMapID = 0;
	depthTest = true;
	depthMask = true;
	alphaTest = false;
	cullFace = true;
	blend = false;
	visible = true;
}

void ObjectRenderParams::applyMTL(boost::property_tree::ptree &MTLtree)
{
	auto tree = MTLtree.get_child_optional("lightPos");
	if (tree)
	{
		additionalUniforms["lightPos"] = JSONVectorReader::readVector4f(tree.get());
	}

	tree = MTLtree.get_child_optional("ambientLight");
	if (tree)
	{
		additionalUniforms["ambientLight"] = JSONVectorReader::readVector3f(tree.get());
	}

	tree = MTLtree.get_child_optional("diffuseLight");
	if (tree)
	{
		additionalUniforms["diffuseLight"] = JSONVectorReader::readVector3f(tree.get());
	}

	tree = MTLtree.get_child_optional("specularLight");
	if (tree)
	{
		additionalUniforms["specularLight"] = JSONVectorReader::readVector3f(tree.get());
	}

	auto floatRecord = MTLtree.get_optional<float>("specularPower");
	if (floatRecord)
	{
		additionalUniforms["specularPower"] = floatRecord.get();
	}

	auto stringRecord = MTLtree.get_optional<std::string>("normalMap");
	if (stringRecord)
	{
		normalMap = stringRecord.get();
	}
}

boost::property_tree::ptree ObjectRenderParams::getPTree(ExtraRenderParamsData &data)
{
	boost::property_tree::ptree result;

	auto originFile = fs::path(data.root->resourcesRoot / shadersSources.first);
	auto copiedFile = fs::path(data.root->filterFolder / fs::path(shadersSources.first).filename());
	if (!equivalent(originFile, copiedFile))
	{
		fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
	}

	originFile = fs::path(data.root->resourcesRoot / shadersSources.second);
	copiedFile = fs::path(data.root->filterFolder / fs::path(shadersSources.second).filename());
	if (!equivalent(originFile, copiedFile))
	{
		fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
	}

	if (!normalMap.empty())
	{
		originFile = fs::path(data.root->resourcesRoot / normalMap);
		copiedFile = fs::path(data.root->filterFolder / fs::path(normalMap).filename());
		if (!equivalent(originFile, copiedFile))
		{
			fs::copy_file(originFile, copiedFile, fs::copy_option::overwrite_if_exists);
		}
	}

	shadersSources.first = fs::path(shadersSources.first).filename().string();
	shadersSources.second = fs::path(shadersSources.second).filename().string();
	normalMap = fs::path(normalMap).filename().string();

	result.put("vertexShader", shadersSources.first);
	result.put("fragmentShader", shadersSources.second);

	if (!visible) result.put("visible", "false");
	if (!depthTest) result.put("depthTest", "false");
	if (!cullFace) result.put("cullFace", "false");
	if (blend) result.put("blend", "true");
	if (alphaTest) result.put("alphaTest", "true");

	if (!normalMap.empty()) result.put("normalMap", normalMap);

	boost::property_tree::ptree uniformsTree;
	static ObjectRenderParamVisitor_toPTree visitor;

	for (auto uniform : additionalUniforms)
	{
		visitor.uniformName = uniform.first;
		uniformsTree.push_back(std::make_pair("", boost::apply_visitor(visitor, uniform.second)));
	}

	result.put_child("uniforms", uniformsTree);

	return result;
}

bool ObjectRenderParams::operator == (ObjectRenderParams &other) const
{
	return shader == other.shader &&
		depthMask == other.depthMask &&
		alphaTest == other.alphaTest &&
		depthTest == other.depthTest &&
		blend == other.blend &&
		cullFace == other.cullFace &&
		visible == other.visible &&
		normalMap == other.normalMap &&
		cameraPos == other.cameraPos &&
		lightPos == other.lightPos &&
		ambientLight == other.ambientLight	&&
		diffuseLight == other.diffuseLight &&
		specularLight == other.specularLight &&
		specularPower == other.specularPower && 
		materialColor == other.materialColor &&
		reflectionRatio == other.reflectionRatio &&
		additionalUniforms == other.additionalUniforms;
}

ShaderSetter::ShaderSetter()
{
}

ShaderSetter::ShaderSetter(ShaderSetter&& from)
{
	shader = from.shader;

	from.shader.reset();
}

ShaderSetter& ShaderSetter::operator=(ShaderSetter&& from)
{
	//Prevent self-assigning:
	if (this != &from)
	{
		shader = from.shader;

		from.shader.reset();
	}
	return *this;
}

ShaderSetter::ShaderSetter(std::shared_ptr<cwc::glShader> newShader, OpenGLModel& model)
{
	shader = newShader;

	if (shader)
	{
		shader->begin();

		model.renderParams.SetUniforms(*shader);
	}

}


ShaderSetter::~ShaderSetter()
{
	if (shader)
	{
		shader->end();
	}
}



VertexAttribSetter::VertexAttribSetter()
{

}

VertexAttribSetter::VertexAttribSetter(VertexAttribSetter&& from)
	: loc(from.loc)
{
	from.loc = -1;
}

VertexAttribSetter& VertexAttribSetter::operator=(VertexAttribSetter&& from)
{
	loc = from.loc;

	from.loc = -1;

	return *this;
}

VertexAttribSetter::VertexAttribSetter(cwc::glShader& shader, const char* attribName, int attribSize, GLuint buffer)
{
	loc = shader.getVetrexShaderAttribLocation(const_cast<char*>(attribName));
	if (loc >= 0)
	{
		glEnableVertexAttribArray(loc);
		shader.BindAttribLocation(loc, const_cast<char*>(attribName));

		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		glVertexAttribPointer(loc, attribSize, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	}
}

VertexAttribSetter::~VertexAttribSetter()
{
	if (loc >= 0)
	{
		glDisableVertexAttribArray(loc);
	}
}

VertexAttribSetter VertexAttribSetter::PositionAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return VertexAttribSetter(shader, "VertexPosition", 3, buffer);
}

VertexAttribSetter VertexAttribSetter::TexCoordAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return (VertexAttribSetter(shader, "VertexTexCoord", 2, buffer));
}

VertexAttribSetter VertexAttribSetter::ColorAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return (VertexAttribSetter(shader, "VertexColor", 4, buffer));
}

VertexAttribSetter VertexAttribSetter::NormalAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return (VertexAttribSetter(shader, "VertexNormal", 3, buffer));
}

VertexAttribSetter VertexAttribSetter::TangentAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return (VertexAttribSetter(shader, "VertexTangent", 3, buffer));
}

VertexAttribSetter VertexAttribSetter::BitangentAttribSetter(cwc::glShader& shader, GLuint buffer)
{
	return (VertexAttribSetter(shader, "VertexBitangent", 3, buffer));
}

void ModelRenderParams::SetUniforms(cwc::glShader& shader)
{
	shader.setUniformMatrix4fv("ModelViewMatrix", 1, false, modelView.data());
	shader.setUniformMatrix4fv("ProjectionMatrix", 1, false, projection.data());
	shader.setUniformMatrix3fv("rotate3x3", 1, false, yawMatrix.data());
	shader.setUniformMatrix3fv("ModelViewMatrix3x3", 1, false, modelView.block<3, 3>(0, 0).data());
	shader.setUniformMatrix3fv("RotationMatrix", 1, false, rotationMatrix.data());


	//int lastTextureIndex = 0;

	GLint uniformLocation = -1;

	uniformLocation = shader.GetUniformLocation("Texture");

	if (uniformLocation != -1)
	{
		glUniform1i(uniformLocation, 0);
		//lastTextureIndex++;
	}


	uniformLocation = shader.GetUniformLocation("Skybox");

	if (uniformLocation != -1)
	{
		glUniform1i(uniformLocation, 1);
		//lastTextureIndex++;
	}


	uniformLocation = shader.GetUniformLocation("NormalMap");

	if (uniformLocation != -1)
	{
		glUniform1i(uniformLocation, 2);
		//lastTextureIndex++;
	}
}


const std::string OpenGLModel::TYPE_NAME = "OpenGLModel";

std::string OpenGLModel::getTypeName()
{
	return TYPE_NAME;
}

JoinedParticleEffect::JoinedParticleEffect() {}
JoinedParticleEffect::JoinedParticleEffect(const JoinedParticleEffect& other) {}

void JoinedParticleEffect::parse(pt::ptree& record)
{
	std::string path;
	try
	{
		path = record.get<std::string>("JSONsource");

		pt::ptree effectTree;
		boost::property_tree::json_parser::read_json(path, effectTree);

		effect.parse(effectTree);
	}
	catch (...)
	{
		qDebug().noquote() << "\nFAILED TO LOAD PARTICLE EFFECT: " << path.c_str() << " \n";
		return;
	}

	auto optional = record.get_child_optional("coords");
	effect.setCoords(optional ? JSONVectorReader::readVector3f(optional.get()) : Vector3f(0, 0, 0));
	coords << effect.getCoords();
	autoCoords << coords;

	enable_rotation = record.get<bool>("enable_rotation", false);
	enable_effect_gravity = record.get<bool>("enable_effect_gravity", true);
	enable_global_gravity = record.get<bool>("enable_global_gravity", true);

	effect.setDepthTest(record.get<bool>("depthTest", true));

	optional = record.get_child_optional("globalGravity");
	globalGravity = optional ? JSONVectorReader::readVector3f(optional.get()) : Vector3f(0, 0, 0);
	autoGlobalGravity = globalGravity;
		
	scale = record.get<float>("scale", 1);
}

OpenGLModel::OpenGLModel()
{
	visible.fill(true);
	useAlphaMask.fill(false);
	animateTextures.fill(false);
	animateTexturesSpeed.fill(0);
	animateTexturesCount.fill(0);
	useHardCodedUniforms = true;
	transformVersion = TransformVersion::V2;
}

void OpenGLModel::onInputFrameResized()
{
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		renderParams.faceXCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		renderParams.faceYCenterSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 30 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		renderParams.faceWidthSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60 * 1280.0 / Resolutions::INPUT_ACTUAL_WIDTH;
		renderParams.pitchSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		renderParams.yawSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
		renderParams.rollSmoother[i].SNAP_MULTIPLIER_SIMPLE = 1.0 / 60;
	}
}

void OpenGLModel::SetUniformsForObject(cwc::glShader& shader, size_t i, size_t face)
{
	if (objectRenderParams[face].size() == 0)
	{
		return;
	}

	size_t index = i;

	if (index > objectRenderParams[face].size() - 1)
	{
		index = objectRenderParams[face].size() - 1;
	}

	glDepthMask(objectRenderParams[face][index]->depthMask);

	if (objectRenderParams[face][index]->alphaTest)
	{
		glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}

	if (objectRenderParams[face][index]->depthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	if (objectRenderParams[face][index]->blend)
	{
		glEnable(GL_BLEND);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	if (objectRenderParams[face][index]->cullFace)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	
	if (useHardCodedUniforms)
	{
		shader.setUniform4fv("cameraPos", 1, &objectRenderParams[face][index]->cameraPos[0]);
		shader.setUniform4fv("lightPos", 1, &objectRenderParams[face][index]->lightPos[0]);

		shader.setUniform3fv("ambientLight", 1, &objectRenderParams[face][index]->ambientLight[0]);
		shader.setUniform3fv("diffuseLight", 1, &objectRenderParams[face][index]->diffuseLight[0]);
		shader.setUniform3fv("specularLight", 1, &objectRenderParams[face][index]->specularLight[0]);

		shader.setUniform1f("specularPower", objectRenderParams[face][index]->specularPower);

		shader.setUniform1f("reflectionRatio", objectRenderParams[face][index]->reflectionRatio);

		shader.setUniform4fv("materialColor", 1, &objectRenderParams[face][index]->materialColor[0]);
	}

	static ObjectRenderParamVisitor_GLSetter visitor;

	visitor.usedShader = objectRenderParams[face][index]->shader;

	for (auto &uniform : objectRenderParams[face][index]->additionalUniforms)
	{
		visitor.uniformName = uniform.first;
		boost::apply_visitor(visitor, uniform.second);
	}
}

void OpenGLModel::loadFromJSON(boost::property_tree::ptree& modelRecord)
{
	name = modelRecord.get<std::string>("name", "");
	canSwapSuit = modelRecord.get<bool>("canSwapSuit", true);

	auto particleList = modelRecord.get_child_optional("particles");
	if (particleList)
	{
		for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		{
			particles[i].resize(particleList.get().size());
			int p = 0;
			for (auto &particle : particleList.get())
			{
				particles[i][p++].parse(particle.second);
			}
		}
	}
}

boost::property_tree::ptree OpenGLModel::getPTree(ExtraModelData &data)
{
	boost::property_tree::ptree result;

	result.put("name", name);

	if (!canSwapSuit)
	{
		result.put("canSwapSuit", "false");
	}

	return result;
}

boost::property_tree::ptree OpenGLModel::findMatchingRenderParamsIndexes(TCommonRenderParams &commonRenderParams,
	ExtraModelData& modelData)
{
	boost::property_tree::ptree renderIDs;

	auto renderParams = modelData.renderParams.begin();

	for (auto target : objectRenderParams[0])
	{
		auto ptr = std::find_if(commonRenderParams.begin(), commonRenderParams.end(), [target](std::shared_ptr<ObjectRenderParams> A)
			-> bool {
			return *target == *A;
		});

		int renderParamsID;

		if (ptr == commonRenderParams.end())
		{
			renderParamsID = commonRenderParams.size();
			commonRenderParams.push_back(target);
			modelData.root->renderParams.push_back(*renderParams++);
		}
		else
		{
			renderParamsID = std::distance(commonRenderParams.begin(), ptr);
		}

		boost::property_tree::ptree renderID;
		renderID.put("", renderParamsID);

		renderIDs.push_back(std::make_pair("", renderID));
	}

	return renderIDs;
}

void OpenGLModel::prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData)
{
	
}

void OpenGLModel::applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
	bool loadTexturesImmediately) // loadTexturesImmediately = true, if filter is started
{
	visible[faceIndex] = suit.get<bool>("visible", true);
	useAlphaMask[faceIndex] = suit.get<bool>("useAlphaMask", false);
	animateTextures[faceIndex] = suit.get<bool>("animateTextures", false);
}

bool OpenGLModel::load()
{
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		for (auto &particle : particles[i])
		{
			particle.lastCoords = { 0, 0, 0 };
			particle.effect.setDirection({ 0, 0 });
			particle.effect.setSpeed({ 0, 0, 0 });

			particle.effect.load();
			particle.effect.startSpawn();
		}
	}

	return true;
}

void OpenGLModel::unload()
{
	for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
	{
		for (auto &particle : particles[i])
		{
			particle.effect.unload();
			particle.effect.freeze();
		}
	}

	for (auto& obj : objects)
	{
		if (obj.vb != 0)
		{
		 	glDeleteBuffers(1, &obj.vb);
		}
		 
		if (obj.nb != 0)
		{
		 	glDeleteBuffers(1, &obj.nb);
		}
		 
		if (obj.cb != 0)
		{
		 	glDeleteBuffers(1, &obj.cb);
		}
		 
		if (obj.tb != 0)
		{
		 	glDeleteBuffers(1, &obj.tb);
		}
		 
		if (obj.tangentb != 0)
		{
		 	glDeleteBuffers(1, &obj.tangentb);
		}
		 
		if (obj.bitangentb != 0)
		{
		 	glDeleteBuffers(1, &obj.bitangentb);
		}
	}

	objects.clear();
}

void OpenGLModel::drawOnCVFrame_OLD(cv::Mat frame, FXModel& face)
{
	// empty
}

std::shared_ptr<ObjectRenderParams> OpenGLModel::createDefaultObjectRenderParams()
{
	return nullptr;
}

void OpenGLModel::setDefaultObjectRenderParams(ObjectRenderParams& params)
{
	// empty
}