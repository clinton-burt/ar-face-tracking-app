#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include "boost/variant.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/filesystem.hpp"

#include <Eigen/Geometry>

#include <vector>
#include <iostream>
#include <array>
#include <memory>

#include "glsl.h"

#include <FrameManager.h>
extern FrameManager frameManager;

class OpenGLModel;
class ObjectRenderParams;

typedef std::vector<std::shared_ptr<ObjectRenderParams>> TCommonRenderParams;

#include "utils/JSONVectorReader.h"
#include "models/Uniforms.h"

#include <models\OpenGLMesh.h>
#include <models\DrawObjectsModel.h>
#include "models\FXModel.h"

#include "Resolutions.h"
#include "VersionsEnums.h"
#include "CommonClasses.h"

#include <boost/format.hpp>

extern boost::format StructUniformFieldFormatter1;
extern boost::format StructUniformFieldFormatter2;

#define StructUniformField1(name, index, field) \
	const_cast<char*>((StructUniformFieldFormatter1 % name % index % field).str().c_str())

#define StructUniformField2(name, index1, field1, index2, field2) \
	const_cast<char*>((StructUniformFieldFormatter2 % name % index1 % field1 % index2 % field2).str().c_str()

class ModelRenderParams
{
public:
	ModelRenderParams()
	{
		//for (int i = 0; i < FaceTracker::MAX_TO_TRACK; ++i)
		//{
		//	faceXCenterSmoother[i].ACTIVITY_THRESHOLD = 15.0;
		//	faceYCenterSmoother[i].ACTIVITY_THRESHOLD = 15.0;

		//	rollSmoother[i].ACTIVITY_THRESHOLD = 2.0;
		//	pitchSmoother[i].ACTIVITY_THRESHOLD = 2.0;
		//	yawSmoother[i].ACTIVITY_THRESHOLD = 2.0;

		//	faceWidthSmoother[i].ACTIVITY_THRESHOLD = 6.0;
		//}
	}

	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> rollSmoother;
	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> pitchSmoother;
	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> yawSmoother;

	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> faceXCenterSmoother;
	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> faceYCenterSmoother;
	std::array<ValueSmoother, FaceTracker::MAX_TO_TRACK> faceWidthSmoother;

	Eigen::Matrix4f modelView;
	Eigen::Matrix4f projection;
	Eigen::Matrix3f yawMatrix;
	Eigen::Matrix3f rotationMatrix;

	std::array<Eigen::Matrix4f, 1> additionalMatrices4;

	void SetUniforms(cwc::glShader& shader);
};

class ObjectRenderParams
{
public:
	std::shared_ptr<cwc::glShader> shader;
	std::pair <std::string, std::string> shadersSources;

	bool alphaTest;
	bool depthMask; // glDepthMask(..);
	bool depthTest;
	bool cullFace;
	bool blend;
	bool visible;

	std::string normalMap;
	GLuint normalMapID;

	Eigen::Vector4f cameraPos;
	Eigen::Vector4f lightPos;

	Eigen::Vector3f ambientLight;
	Eigen::Vector3f diffuseLight;
	Eigen::Vector3f specularLight;
	float specularPower;

	Eigen::Vector4f materialColor;

	float reflectionRatio;

	std::map<std::string, TUniformVariant> additionalUniforms;

	ObjectRenderParams();

	ObjectRenderParams(boost::property_tree::ptree& renderParams, fs::path& resourcesRoot);

	void reset();
	void applyMTL(boost::property_tree::ptree &MTLtree);
	boost::property_tree::ptree getPTree(ExtraRenderParamsData &data);

	bool operator == (ObjectRenderParams &other) const;
};


class OpenGLModel;


class ShaderSetter {
public:

	ShaderSetter();

	ShaderSetter(std::shared_ptr<cwc::glShader> newShader, OpenGLModel& model);

	ShaderSetter(const ShaderSetter& from) = delete;

	ShaderSetter(ShaderSetter&& from);

	ShaderSetter& operator=(const ShaderSetter& from) = delete;

	ShaderSetter& operator=(ShaderSetter&& from);

	~ShaderSetter();

protected:
	std::shared_ptr<cwc::glShader> shader;

};

class VertexAttribSetter
{
public:
	VertexAttribSetter();

	VertexAttribSetter(const VertexAttribSetter& from) = delete;

	VertexAttribSetter(VertexAttribSetter&& from);

	VertexAttribSetter& operator=(const VertexAttribSetter& from) = delete;

	VertexAttribSetter& operator=(VertexAttribSetter&& from);

	VertexAttribSetter(cwc::glShader& shader, const char* attribName, int attribSize, GLuint buffer);

	~VertexAttribSetter();

	static VertexAttribSetter PositionAttribSetter(cwc::glShader& shader, GLuint buffer);
	static VertexAttribSetter TexCoordAttribSetter(cwc::glShader& shader, GLuint buffer);
	static VertexAttribSetter ColorAttribSetter(cwc::glShader& shader, GLuint buffer);
	static VertexAttribSetter NormalAttribSetter(cwc::glShader& shader, GLuint buffer);
	static VertexAttribSetter TangentAttribSetter(cwc::glShader& shader, GLuint buffer);
	static VertexAttribSetter BitangentAttribSetter(cwc::glShader& shader, GLuint buffer);

protected:
	int loc = -1;
};

struct JoinedParticleEffect
{
	ParticleEffect effect;
	Vector3f autoCoords;
	Vector3f coords;
	float scale;
	float autoScale;
	Vector3f globalGravity;
	Vector3f autoGlobalGravity;
	bool enable_rotation;
	bool enable_effect_gravity;
	bool enable_global_gravity;

	Vector3f lastCoords;

	JoinedParticleEffect();
	JoinedParticleEffect(const JoinedParticleEffect& other);
	void parse(pt::ptree& record);
};

class OpenGLModel
{
public:

	OpenGLModel();

	std::string name;

	TransformVersion transformVersion;

	std::vector<DrawObjectsModel> objects;
	std::array<std::vector<std::shared_ptr<ObjectRenderParams>>, 5> objectRenderParams;
	ModelRenderParams renderParams;

	float depth = 0;

	std::array<bool, FaceTracker::MAX_TO_TRACK> visible;
	std::array<bool, FaceTracker::MAX_TO_TRACK> useAlphaMask;
	std::array<bool, FaceTracker::MAX_TO_TRACK> animateTextures;
	std::array<size_t, FaceTracker::MAX_TO_TRACK> animateTexturesCount;
	std::array<size_t, FaceTracker::MAX_TO_TRACK> animateTexturesSpeed;

	bool canSwapSuit = true; // if false, ignore this model and apply current suit to a next model (ignore depthMask, for example)

	bool useHardCodedUniforms = true;

	void SetUniformsForObject(cwc::glShader& shader, size_t i, size_t face);

	template<class T>
	static std::shared_ptr<OpenGLModel> create()
	{
		return std::make_shared<T>();
	}

	static const std::string TYPE_NAME;
	virtual std::string getTypeName();

	virtual void onInputFrameResized();

	virtual void loadFromJSON(boost::property_tree::ptree& modelRecord);

	virtual boost::property_tree::ptree getPTree(ExtraModelData &data);
	virtual void prepareSuitForJSON(boost::property_tree::ptree &suit, ExtraModelData& modelData);

	boost::property_tree::ptree findMatchingRenderParamsIndexes(TCommonRenderParams &commonRenderParams, ExtraModelData& modelData);

	virtual void applySuit(boost::property_tree::ptree& suit, size_t faceIndex, TCommonRenderParams &commonRenderParams,
		bool loadTexturesImmediately);

	virtual bool load();
	virtual void transform(FXModel& face, ExternalRenderParams &externalRenderParams) = 0;
	virtual void draw(FXModel& face, ExternalRenderParams &externalRenderParams) = 0;
	virtual void unload();

	virtual void drawOnCVFrame_OLD(cv::Mat frame, FXModel& fxModel);

	virtual std::shared_ptr<ObjectRenderParams> createDefaultObjectRenderParams();
	virtual void setDefaultObjectRenderParams(ObjectRenderParams& params);

	typedef std::pair<std::string, std::pair<std::string, std::string>> DefaultShadersMapElement;
	static const std::vector<std::pair<std::string, std::pair<std::string, std::string>>> DefaultShadersMap;

	std::array<std::vector<JoinedParticleEffect>, FaceTracker::MAX_TO_TRACK> particles;
};

#include "FxCore.h" // after OpenGlModel

#endif