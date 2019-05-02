#ifndef PARTICLES_H_INCLUDED
#define PARTICLES_H_INCLUDED

#include <glsl.h>

#include <chrono>
#include <boost\property_tree\ptree.hpp>
#include <boost\filesystem.hpp>

#include "particles\ParticleDataTypes.h"

namespace chrono = std::chrono;
namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

struct Matrix3fWrapper
{
	bool valued;
	Matrix3f M;

	Matrix3fWrapper();
	inline void set(Matrix3f& M);
	inline void set(Matrix3f&& M);
	void mult(Matrix3f& other);
	void mult(Matrix3fWrapper& other);
	inline Matrix4f extent();
};

struct Matrix4fWrapper
{
	bool valued;
	Matrix4f M;

	Matrix4fWrapper();
	inline void set(Matrix4f& M);
	inline void set(Matrix4f&& M);
	inline void set(Matrix3fWrapper& M);
	void mult(Matrix4fWrapper& other);
};

enum class RelatedCoordinateSystem
{
	PARTICLE_EFFECT,
	PARTICLE_EMITTER,
	PARTICLE_BIRTH,
	PARTICLE_ITSELF
};

extern const std::map<std::string, RelatedCoordinateSystem> CoordinateSystemMap;

class CoordinateSystem
{
public:
	virtual float getEndTime() = 0;
	virtual Vector3f getCoords() = 0;
	virtual Vector2f getDirection() = 0; // yaw + roll
	virtual Vector3f getSpeed() = 0;
	virtual Matrix4f getOuterTransformation() = 0;
	virtual Matrix3f getOuterRotation() = 0;

	// input is normalized !
	static Vector2f directionFromSpeed(Vector3f speedDirection);
};

struct ParticleUpdateParams
{
	Matrix4f outer_transformation;
	Matrix4f effect_scaling;
	Matrix3f effect_rotation;
	bool enable_effect_rotation;
	bool enable_global_gravity;
	bool enable_effect_gravity;
	Vector3f globalGravity;
};

class EmitterDescription
{
	friend class Particle;
	friend class ParticleEmitter;

protected:
	mutable GLuint texID;
	mutable std::shared_ptr<cwc::glShader> particleShader;
	mutable GLuint coordsVBO, textureVBO, colorVBO;

	std::vector<Vector3f> coordsArray;
	std::vector<Vector2f> textureArray;
	std::vector<Vector4f> colorArray;

	bool enabled;

	// < Emitter Description >

	uint maxParticles;
	uint preloading;

	std::string texturePath;
	Vector2f textureSize;

	bool hasInertion;
	Vector3f gravity;

	RelatedCoordinateSystem spawnRange_system;

	RandomValueAtTimeline<FloatWrapper, RandomFloat> spawnTotal;
	RandomValueAtTimeline<Vector4fWrapper, RandomVector4f> spawnRange; // yaw + pitch + delta yaw + delta pitch
	// < / Emitter Description >

	// < Particle Description >
	RandomFloat lifeTime;

	RelatedCoordinateSystem coords_system;
	RelatedCoordinateSystem offset_axis_system;
	RelatedCoordinateSystem coords_axis_system;
	RelatedCoordinateSystem rotation_system;

	RandomValueAtTimeline<Vector3fWrapper, RandomVector3f> coords;
	RandomValueAtTimeline<Vector3fWrapper, RandomVector3f> offset;
	RandomValueAtTimeline<Vector3fWrapper, RandomVector3f> rotation; // yaw + pitch + roll
	RandomValueAtTimeline<FloatWrapper, RandomFloat> scale;
	RandomValueAtTimeline<Vector4fWrapper, RandomVector4f> color;

	std::vector<std::shared_ptr<EmitterDescription>> emittersTypes;
	// </ Particle Description >

public:
	EmitterDescription(pt::ptree& JSONsource);

	void load() const;
	void unload() const;
};

class ParticleEmitter;

class ParticleEffect : public CoordinateSystem
{
protected:
	std::vector<ParticleEmitter> emitters;
	std::vector<std::shared_ptr<EmitterDescription>> emittersTypes;

	Vector3f coords;
	Vector2f direction;
	Vector3f speed;

	bool spawning;
	bool showLocation;
	bool inited;

	GLuint VBO;
	std::shared_ptr<cwc::glShader> colorShader;

	float quadSize;
	float effectWidth;
	bool depthTest;

	Matrix4f transformation;
	Matrix4f effect_scaling;
	Matrix3f effect_rotation;

public:
	ParticleEffect();

	void parse(const pt::ptree& JSONsource);

	ParticleEffect(const ParticleEffect& other) = delete;
	ParticleEffect& operator=(const ParticleEffect& other) = delete;
	ParticleEffect(ParticleEffect&& other) = delete;
	ParticleEffect& operator=(ParticleEffect&& other) = delete;

	float getEffectWidth();

	float getEndTime() override;
	Vector3f getCoords() override;
	Vector2f getDirection() override;
	Vector3f getSpeed() override;
	Matrix4f getOuterTransformation() override;
	Matrix3f getOuterRotation() override;

	void setDepthTest(bool enabled);
	void setCoords(Vector3f coords);
	void setDirection(Vector2f direction);
	void setSpeed(Vector3f speed);

	void load(); // load textures
	void update(float dt, ParticleUpdateParams& params); // seconds
	void draw(Matrix4f& projection);
	void unload(); // unload textures

	void startSpawn();
	void stopSpawn();
	bool isSpawning();

	void freeze(); // kill all particles
};

class ParticleBirth : public CoordinateSystem
{
public:
	Matrix3f rotation;
	Matrix4f transformation;

	Vector3f coords;
	Vector2f direction;
	Vector3f inertion;

	float getEndTime() override;
	Vector3f getCoords() override;
	Vector2f getDirection() override;
	Vector3f getSpeed() override;
	Matrix4f getOuterTransformation() override;
	Matrix3f getOuterRotation() override;
};

class ParticleEmitter
{
protected:
	std::vector<std::shared_ptr<Particle>> particles;

	uint spawned;
	float spawnTimer;
	bool spawning;
	bool has_children;

	// < VARIOUS PARAMS TO BE INITED IN LOAD() >
	std::shared_ptr<Interpolator<FloatWrapper>> spawnTotal;
	std::shared_ptr<Interpolator<Vector4fWrapper>> spawnRange; // yaw + pitch + delta yaw + delta pitch
	// < / VARIOUS >

	CoordinateSystem* root_system;
	CoordinateSystem* parent_system;
	CoordinateSystem* spawnRange_system;

	void allocate();

public:
	const std::shared_ptr<EmitterDescription> description;

	ParticleEmitter(std::shared_ptr<EmitterDescription> description);

	void init(
		CoordinateSystem* root_system,
		CoordinateSystem* parent_system,
		CoordinateSystem* spawnRange_system);

	void update(float dt, ParticleUpdateParams& params);
	void draw(Matrix4f& projection);

	ParticleBirth randomBirth();
	void spawn(uint count, float dt);

	void startSpawn();
	void stopSpawn();
	bool isSpawning();
	bool hasChildren();

	void freeze();

	CoordinateSystem* getParticleSystem(RelatedCoordinateSystem flag, Particle* particle);
};

class Particle : public CoordinateSystem
{
protected:

	friend CoordinateSystem* ParticleEmitter::getParticleSystem(RelatedCoordinateSystem flag, Particle* particle);

	std::vector<ParticleEmitter> emitters;
	const std::vector<std::shared_ptr<EmitterDescription>> emittersTypes;
	const std::shared_ptr<EmitterDescription> description;

	ParticleBirth birth;

	// < VARIOUS PARAMS TO BE INITED IN LOAD() >
	float lifeTime;
	float deathTime;

	std::shared_ptr<Interpolator<Vector3fWrapper>> rotation; // yaw + pitch + roll
	std::shared_ptr<Interpolator<Vector3fWrapper>> coords;
	std::shared_ptr<Interpolator<Vector3fWrapper>> offset;
	std::shared_ptr<Interpolator<FloatWrapper>> scale;
	std::shared_ptr<Interpolator<Vector4fWrapper>> color;
	// < / VARIOUS >

	bool alive;

	CoordinateSystem* root_system;
	CoordinateSystem* parent_system;

	CoordinateSystem* coords_system;
	CoordinateSystem* offset_axis_system;
	CoordinateSystem* coords_axis_system;
	CoordinateSystem* rotation_system;

	void transform(ParticleUpdateParams& params);
	void updateThisSystem();

	Vector3f lastCoords;
	float lastMoment;

	Vector3f currentCoords;
	Vector2f currentDirection;
	Vector3f currentSpeed;

	Matrix4f transformation;

public:
	Particle(const std::shared_ptr<EmitterDescription> description, const std::vector<std::shared_ptr<EmitterDescription>> emittersTypes);

	void init(
		CoordinateSystem* root_system,
		CoordinateSystem* parent_system,
		CoordinateSystem* coords_system,
		CoordinateSystem* offset_axis_system,
		CoordinateSystem* coords_axis_system,
		CoordinateSystem* rotation_system);

	float getEndTime() override;
	Vector3f getCoords() override;
	Vector2f getDirection() override;
	Vector3f getSpeed() override;
	Matrix4f getOuterTransformation() override;
	Matrix3f getOuterRotation() override;

	void update(float dt, ParticleUpdateParams& params);
	void draw(Matrix4f& projection);

	bool isAlive();
	void kindle(ParticleBirth birth, float initialTime);

	void freeze();

	CoordinateSystem* getEmitterSystem(RelatedCoordinateSystem flag);
};

#endif