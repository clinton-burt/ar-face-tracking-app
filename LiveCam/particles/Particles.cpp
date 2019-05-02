#include "particles/Particles.h"

#include <QDebug>
#include "utils\ModelLoader.h"

#include "CommonClasses.h"

extern ShaderManagerWrapper shaderManagerWrapper;

const Vector3f X = { 1, 0, 0 };
const Vector3f Y = { 0, 1, 0 };
const Vector3f Z = { 0, 0, 1 };

const std::map<std::string, RelatedCoordinateSystem> CoordinateSystemMap =
{
	{ "PARTICLE_EFFECT", RelatedCoordinateSystem::PARTICLE_EFFECT },
	{ "PARTICLE_EMITTER", RelatedCoordinateSystem::PARTICLE_EMITTER },
	{ "PARTICLE_BIRTH", RelatedCoordinateSystem::PARTICLE_BIRTH },
	{ "PARTICLE_ITSELF", RelatedCoordinateSystem::PARTICLE_ITSELF }
};

// input is normalized !
Vector2f CoordinateSystem::directionFromSpeed(Vector3f speedDirection)
{
	float x = speedDirection[0];
	float y = speedDirection[1];
	float z = speedDirection[2];
	float yaw = -atan2f(z, x);
	float roll = atan2f(y, sqrtf(x * x + z * z));
	return{ yaw, roll };
}

template <typename T, typename R>
void DegToRad(RandomValueAtTimeline<T, R>& vector)
{
	for (auto& elem : vector.timeline)
	{
		elem.v.min = elem.v.min.get() * (float)M_PI / 180;
		elem.v.max = elem.v.max.get() * (float)M_PI / 180;
	}
}

EmitterDescription::EmitterDescription(pt::ptree& JSONsource)
{
	enabled = JSONsource.get<bool>("enabled", true);

	maxParticles = JSONsource.get<uint>("maxParticles", std::numeric_limits<uint>::max());

	texturePath = JSONsource.get<std::string>("texturePath");
	textureSize = JSONVectorReader::readVector2f(JSONsource.get_child("textureSize"));

	hasInertion = JSONsource.get<bool>("hasInertion", false);

	auto option = JSONsource.get_child_optional("gravity");
	gravity = option ? JSONVectorReader::readVector3f(option.get()) : Vector3f(0, 0, 0);

	coords_system = CoordinateSystemMap.at(JSONsource.get<std::string>("coords_system"));
	offset_axis_system = CoordinateSystemMap.at(JSONsource.get<std::string>("offset_axis_system", "PARTICLE_EFFECT"));
	coords_axis_system = CoordinateSystemMap.at(JSONsource.get<std::string>("coords_axis_system"));
	rotation_system = CoordinateSystemMap.at(JSONsource.get<std::string>("rotation_system"));
	spawnRange_system = CoordinateSystemMap.at(JSONsource.get<std::string>("spawnRange_system"));

	auto tree = JSONsource.get_child("spawnTotal");
	spawnTotal.load(tree);

	preloading = JSONsource.get<uint>("preloading", (uint)spawnTotal.timeline.back().v.max.get());

	tree = JSONsource.get_child("spawnRange");
	spawnRange.load(tree);
	DegToRad<Vector4fWrapper, RandomVector4f>(spawnRange);

	tree = JSONsource.get_child("lifeTime");
	lifeTime.load(tree);

	tree = JSONsource.get_child("coords");
	coords.load(tree);

	auto optional = JSONsource.get_child_optional("offset");
	if (optional)
	{
		offset.load(optional.get());
	}
		
	tree = JSONsource.get_child("rotation");
	rotation.load(tree);
	DegToRad<Vector3fWrapper, RandomVector3f>(rotation);

	tree = JSONsource.get_child("scale");
	scale.load(tree);

	tree = JSONsource.get_child("color");
	color.load(tree);

	emittersTypes.clear();

	auto emittersList = JSONsource.get_child_optional("emitters");
	if (emittersList)
	{
		for (auto &emitterRecord : emittersList.get())
		{
			emittersTypes.push_back(std::make_shared<EmitterDescription>(emitterRecord.second));
		}
	}
}

void EmitterDescription::load() const
{
	auto pathCopy = texturePath;
	texID = resourceManager.loadTexture(pathCopy).ID;

	particleShader = shaderManagerWrapper.LoadFromFile("./assets/particles/particle-shader.vertex", "./assets/particles/particle-shader.fragment");

	glGenBuffers(1, &coordsVBO);
	glGenBuffers(1, &textureVBO);
	glGenBuffers(1, &colorVBO);

	for (auto &description : emittersTypes)
	{
		description->load();
	}
}

void EmitterDescription::unload() const
{
	glDeleteBuffers(1, &coordsVBO);
	glDeleteBuffers(1, &textureVBO);
	glDeleteBuffers(1, &colorVBO);
}

ParticleEffect::ParticleEffect() : inited(false), depthTest(false) { }

void ParticleEffect::parse(const pt::ptree& JSONsource)
{
	inited = true;
	showLocation = JSONsource.get<bool>("showLocation", false);

	quadSize = JSONsource.get<float>("quadSize", 1);

	effectWidth = JSONsource.get<float>("effectWidth", Resolutions::OUTPUT_WIDTH);

	spawning = false;

	auto &emittersList = JSONsource.get_child("emitters");
	for (auto &emitterRecord : emittersList)
	{
		emittersTypes.push_back(std::make_shared<EmitterDescription>(const_cast<pt::ptree&>(emitterRecord.second)));
		emitters.push_back(ParticleEmitter(emittersTypes.back()));
		emitters.back().init(this, this, this);
	}
}

float ParticleEffect::getEffectWidth()
{
	return effectWidth;
}

float ParticleEffect::getEndTime()
{
	return -1;
}

Vector3f ParticleEffect::getCoords()
{
	return coords;
}

Vector2f ParticleEffect::getDirection()
{
	return direction;
}

Vector3f ParticleEffect::getSpeed()
{
	return speed;
}

Matrix4f ParticleEffect::getOuterTransformation()
{
	return transformation;
}

Matrix3f ParticleEffect::getOuterRotation()
{
	return effect_rotation;
}

void ParticleEffect::setDepthTest(bool enabled)
{
	depthTest = enabled;
}

void ParticleEffect::setCoords(Vector3f coords)
{
	this->coords = coords;
}

void ParticleEffect::setDirection(Vector2f direction)
{
	this->direction = direction;
}

void ParticleEffect::setSpeed(Vector3f speed)
{
	this->speed = speed;
}

void ParticleEffect::load()
{
	if (!inited)
	{
		qDebug() << "Particle effect is not inited.";
		return;
	}

	std::array<Vector3f, 4> vb =
	{
		Vector3f(-quadSize, -quadSize, 0), Vector3f(quadSize, -quadSize, 0),
		Vector3f(quadSize, quadSize, 0), Vector3f(-quadSize, quadSize, 0)
	};

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(Vector3f), vb.data(), GL_STATIC_DRAW);

	colorShader = shaderManagerWrapper.LoadFromFile("./assets/particles/color-shader.vertex", "./assets/particles/color-shader.fragment");

	for (auto &emitter : emitters)
	{
		emitter.description->load();
	}
}

void ParticleEffect::update(float dt, ParticleUpdateParams& params)
{
	if (!inited)
	{
		qDebug() << "Particle effect is not inited.";
		return;
	}

	transformation = params.outer_transformation;
	this->effect_scaling = params.effect_scaling;
	this->effect_rotation = params.effect_rotation;

	for (auto &emitter : emitters)
	{
		emitter.update(dt, params);
	}
}

void ParticleEffect::draw(Matrix4f& projection)
{
	if (!inited)
	{
		qDebug() << "Particle effect is not inited.";
		return;
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	if (depthTest)
	{
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	if (showLocation)
	{

		colorShader->begin();

		Matrix4f MVP = projection * transformation * Affine3f(Translation3f(coords)).matrix() * effect_scaling;

		colorShader->setUniformMatrix4fv("ProjectionMatrix", 1, false, MVP.data());

		Vector4f color = spawning ? Vector4f(0, 1, 0, 1) : Vector4f(1, 0, 0, 1);
		colorShader->setUniform4fv("Color", 1, color.data());

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*colorShader, VBO));

		glDrawArrays(GL_QUADS, 0, 6);

		colorShader->end();
	}

	for (auto &emitter : emitters)
	{
		emitter.draw(projection);
	}

	glPopAttrib();
}

void ParticleEffect::unload()
{
	for (auto &emitter : emitters)
	{
		emitter.description->unload();
	}
}

void ParticleEffect::startSpawn()
{
	spawning = true;

	for (auto &emitter : emitters)
	{
		emitter.startSpawn();
	}
}

void ParticleEffect::stopSpawn()
{
	spawning = false;

	for (auto &emitter : emitters)
	{
		emitter.stopSpawn();
	}
}

bool ParticleEffect::isSpawning()
{
	return spawning;
}

void ParticleEffect::freeze()
{
	spawning = false;

	for (auto &emitter : emitters)
	{
		emitter.freeze();
	}
}

ParticleEmitter::ParticleEmitter(const std::shared_ptr<EmitterDescription> description)
	: description(description)
{
	spawning = false;
	has_children = false;
}

void ParticleEmitter::init(
	CoordinateSystem* root_system,
	CoordinateSystem* parent_system,
	CoordinateSystem* spawnRange_system)
{
	this->root_system = root_system;
	this->parent_system = parent_system;
	this->spawnRange_system = spawnRange_system;

	allocate();
}

bool ParticleEmitter::isSpawning()
{
	return spawning;
}

void ParticleEmitter::startSpawn()
{
	spawnTimer = 0;
	spawned = 0;

	float matchParentDeathTime = description->spawnTotal.matchDeathTime ? parent_system->getEndTime() : -1;
	spawnTotal = description->spawnTotal.random(matchParentDeathTime);

	matchParentDeathTime = description->spawnRange.matchDeathTime ? parent_system->getEndTime() : -1;
	spawnRange = description->spawnRange.random(matchParentDeathTime);

	spawning = true;
}

void ParticleEmitter::stopSpawn()
{
	spawning = false;
}

bool ParticleEmitter::hasChildren()
{
	return has_children;
}

void ParticleEmitter::update(float dt, ParticleUpdateParams& params)
{
	if (!description->enabled)
	{
		return;
	}

	if (spawning)
	{
		spawnTimer += dt;

		uint total;
		bool rollback = false;

		if (description->spawnTotal.timelineType != TimelineType::CONTINUED &&
			spawnTimer > spawnTotal->knots.back().t)
		{
			total = spawnTotal->knots.back().v.get();

			if (description->spawnTotal.timelineType == TimelineType::LOOPED)
			{
				rollback = true;
			}
		}
		else
		{
			total = spawnTotal->at(spawnTimer).get();
		}

		if (spawned < total)
		{
			spawn(total - spawned, dt);
			spawned = total;
		}

		if (rollback)
		{
			spawnTimer -= spawnTotal->knots.back().t;
			spawned = 0;
		}
	}

	has_children = false;

	int i = 0;

	for (auto &particle : particles)
	{
		particle->update(dt, params);

		if (!has_children && particle->isAlive())
		{
			has_children = true;
		}

		++i;
	}
}

void ParticleEmitter::draw(Matrix4f& projection)
{
	if (!description->enabled)
	{
		return;
	}

	description->coordsArray.clear();
	description->textureArray.clear();
	description->colorArray.clear();

	for (auto &particle : particles)
	{
		particle->draw(projection);
	}

	if (has_children)
	{
		
		description->particleShader->begin();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, description->texID);
		description->particleShader->setUniform1i("Texture", 0);

		description->particleShader->setUniformMatrix4fv("ProjectionMatrix", 1, false, projection.data());

		VertexAttribSetter vPosition(VertexAttribSetter::PositionAttribSetter(*description->particleShader, description->coordsVBO));
		VertexAttribSetter vTexCoords(VertexAttribSetter::TexCoordAttribSetter(*description->particleShader, description->textureVBO));
		VertexAttribSetter vColor(VertexAttribSetter::ColorAttribSetter(*description->particleShader, description->colorVBO));

		glBindBuffer(GL_ARRAY_BUFFER, description->coordsVBO);
		glBufferData(GL_ARRAY_BUFFER, description->coordsArray.size() * sizeof(Vector3f), description->coordsArray.data(), GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, description->textureVBO);
		glBufferData(GL_ARRAY_BUFFER, description->textureArray.size() * sizeof(Vector2f), description->textureArray.data(), GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, description->colorVBO);
		glBufferData(GL_ARRAY_BUFFER, description->colorArray.size() * sizeof(Vector4f), description->colorArray.data(), GL_STREAM_DRAW);


		glDrawArrays(GL_QUADS, 0, description->coordsArray.size());

		description->particleShader->end();
	}
}

void ParticleEmitter::allocate()
{
	int N = particles.size() + description->preloading > description->maxParticles
		? description->maxParticles
		: particles.size() + description->preloading;

	particles.reserve(N);
	description->coordsArray.reserve(4 * N);
	description->textureArray.reserve(4 * N);
	description->colorArray.reserve(4 * N);

	N -= particles.size();

	for (int i = 0; i < N; ++i)
	{
		particles.push_back(std::make_shared<Particle>(description, description->emittersTypes));
		Particle* particle = particles.back().get();
		particle->init(root_system, parent_system,
			getParticleSystem(description->coords_system, particle),
			getParticleSystem(description->offset_axis_system, particle),
			getParticleSystem(description->coords_axis_system, particle),
			getParticleSystem(description->rotation_system, particle));
	}
}

ParticleBirth ParticleEmitter::randomBirth()
{
	ParticleBirth birth;
	birth.coords = parent_system->getCoords();
	birth.direction = parent_system->getDirection();
	birth.inertion = description->hasInertion ? parent_system->getSpeed() : Vector3f(0, 0, 0);
	birth.transformation = parent_system->getOuterTransformation();
	birth.rotation = parent_system->getOuterRotation();

	auto cur_range = spawnRange->at(spawnTimer).get();
	float deltaYaw = SalmonRandom::RandomReal(-cur_range[2] / 2, cur_range[2] / 2);
	float deltaRoll = SalmonRandom::RandomReal(-cur_range[3] / 2, cur_range[3] / 2);

	birth.direction += Vector2f(cur_range[0] + deltaYaw, cur_range[1] + deltaRoll);


	return birth;
}

void ParticleEmitter::spawn(uint count, float dt)
{
	auto unkindled = particles.begin();

	uint i = 0;

	float timeStep = spawnTotal->getType() == InterpolatorType::DISCRETE ? 0 : dt / count;
	float timeOffset = -dt;

	while (i < count)
	{
		for (; i < count; ++i)
		{
			for (; unkindled != particles.end(); ++unkindled)
			{
				if (!(*unkindled)->isAlive())
				{
					break;
				}
			}

			if (unkindled == particles.end())
			{
				break;
			}

			auto birth = randomBirth();

			(*unkindled++)->kindle(birth, timeOffset);

			timeOffset += timeStep;
		}

		if (i < count)
		{
			if (particles.size() < description->maxParticles)
			{
				int currentSize = particles.size();
				allocate();
				unkindled = particles.begin() + currentSize;
			}
			else
			{
				break;
			}
		}
	}
}

CoordinateSystem* ParticleEmitter::getParticleSystem(RelatedCoordinateSystem flag, Particle* particle)
{
	switch (flag)
	{
	case RelatedCoordinateSystem::PARTICLE_EFFECT: return root_system;
	case RelatedCoordinateSystem::PARTICLE_EMITTER: return parent_system;
	case RelatedCoordinateSystem::PARTICLE_BIRTH: return &particle->birth;
	case RelatedCoordinateSystem::PARTICLE_ITSELF: return particle;
	}
}

void ParticleEmitter::freeze()
{
	spawning = false;

	for (auto &particle : particles)
	{
		particle->freeze();
	}
}

float ParticleBirth::getEndTime()
{
	return -1;
}

Vector3f ParticleBirth::getCoords()
{
	return coords;
}

Vector2f ParticleBirth::getDirection()
{
	return direction;
}

Vector3f ParticleBirth::getSpeed()
{
	return inertion;
}

Matrix4f ParticleBirth::getOuterTransformation()
{
	return transformation;
}

Matrix3f ParticleBirth::getOuterRotation()
{
	return rotation;
}

Particle::Particle(const std::shared_ptr<EmitterDescription> description,
	const std::vector<std::shared_ptr<EmitterDescription>> emittersTypes)
	: description(description)
	, emittersTypes(emittersTypes)
{
	alive = false;

	for (auto &description : emittersTypes)
	{
		emitters.push_back(ParticleEmitter(description));
	}
}

void Particle::init(
	CoordinateSystem* root_system,
	CoordinateSystem* parent_system,
	CoordinateSystem* coords_system,
	CoordinateSystem* ofset_axis_system,
	CoordinateSystem* coords_axis_system,
	CoordinateSystem* rotation_system)
{
	this->root_system = root_system;
	this->parent_system = parent_system;
	this->coords_system = coords_system;
	this->offset_axis_system = ofset_axis_system;
	this->coords_axis_system = coords_axis_system;
	this->rotation_system = rotation_system;

	for (auto &emitter : emitters)
	{
		emitter.init(root_system, this,
			getEmitterSystem(description->spawnRange_system));
	}
}

float Particle::getEndTime()
{
	return deathTime;
}

Vector3f Particle::getCoords()
{
	return currentCoords;
}

Vector2f Particle::getDirection()
{
	return currentDirection;
}

Vector3f Particle::getSpeed()
{
	return currentSpeed;
}

Matrix4f Particle::getOuterTransformation()
{
	return birth.transformation;
}

Matrix3f Particle::getOuterRotation()
{
	return birth.rotation;
}

bool Particle::isAlive()
{
	return alive;
}

void Particle::kindle(ParticleBirth birth, float initialTime)
{
	if (alive)
	{
		return;
	}

	this->birth = birth;

	alive = true;

	lifeTime = initialTime;

	deathTime = description->lifeTime.random().get();

	float matchDeathTime = description->coords.matchDeathTime ? deathTime : -1;
	coords = description->coords.random(matchDeathTime);

	if (!description->offset.timeline.empty())
	{
		matchDeathTime = description->offset.matchDeathTime ? deathTime : -1;
		offset = description->offset.random(matchDeathTime);
	}

	matchDeathTime = description->rotation.matchDeathTime ? deathTime : -1;
	rotation = description->rotation.random(matchDeathTime);

	matchDeathTime = description->scale.matchDeathTime ? deathTime : -1;
	scale = description->scale.random(matchDeathTime);

	matchDeathTime = description->color.matchDeathTime ? deathTime : -1;
	color = description->color.random(matchDeathTime);

	lastMoment = initialTime;
	lastCoords << coords->at(initialTime).get();

	for (auto &emitter : emitters)
	{
		emitter.startSpawn();
	}
}

void Particle::update(float dt, ParticleUpdateParams& params)
{
	if (!alive)
	{
		return;
	}

	lifeTime += dt;

	if (lifeTime > deathTime)
	{
		bool dead = true;

		for (auto &emitter : emitters)
		{
			emitter.stopSpawn();

			if (emitter.hasChildren())
			{
				dead = false;
				break;
			}
		}

		if (dead)
		{
			alive = false;
		}
	}

	transform(params);
	updateThisSystem();

	for (auto &emitter : emitters)
	{
		emitter.update(dt, params);
	}
}

void Particle::draw(Matrix4f& projection)
{
	if (!alive)
	{
		return;
	}

	if (lifeTime < deathTime)
	{
		Matrix4f globalTransformation = coords_system->getOuterTransformation() * transformation;

		std::array<Vector3f, 4> coords =
		{
			(globalTransformation * Vector4f(-1, -1, 0, 1)).head<3>(),
			(globalTransformation * Vector4f(1, -1, 0, 1)).head<3>(),
			(globalTransformation * Vector4f(1, 1, 0, 1)).head<3>(),
			(globalTransformation * Vector4f(-1, 1, 0, 1)).head<3>()
		};

		const std::array<Vector2f, 4> texCoords =
		{
			Vector2f(0, 0),
			Vector2f(1, 0),
			Vector2f(1, 1),
			Vector2f(0, 1)
		};

		Vector4f color = this->color->at(lifeTime).get();

		if (lifeTime > 0.6 && color == Vector4f( 1, 0, 1, 1 ))
		{
			int s = 0;
		}

		for (int i = 0; i < 4; ++i)
		{
			description->coordsArray.push_back(coords[i]);
			description->textureArray.push_back(texCoords[i]);
			description->colorArray.push_back(color);
		}
	}

	for (auto &emitter : emitters)
	{
		emitter.draw(projection);
	}
}

void Particle::freeze()
{
	alive = false;

	for (auto &emitter : emitters)
	{
		emitter.freeze();
	}
}

#define Vec3_Float(vec3, f) Vector4f(vec3[0], vec3[1], vec3[2], f)

void Particle::updateThisSystem()
{
	currentCoords = (transformation * Vector4f(0, 0, 0, 1)).head<3>();
		
	currentSpeed = currentCoords - lastCoords;
	float speedLength = currentSpeed.norm();

	if (speedLength < 0.001f)
	{
		currentDirection << 0, 0;
		currentSpeed << 0, 0, 0;
	}
	else
	{
		currentDirection << CoordinateSystem::directionFromSpeed(currentSpeed / speedLength);
		currentSpeed /= lifeTime - lastMoment;
	}

	lastMoment = lifeTime;
	lastCoords = currentCoords;
}

void Particle::transform(ParticleUpdateParams& params)
{
	Vector3f rotatedX = X;
	Vector3f rotatedY = Y;
	Vector3f rotatedZ = Z;

	// PARTICLE SCALE
	auto cur_scale = scale->at(lifeTime).get();

	Matrix4fWrapper scaling;
	scaling.valued = true;
	scaling.M <<
		description->textureSize[0] * cur_scale, 0, 0, 0,
		0, description->textureSize[1] * cur_scale, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;

	Matrix3fWrapper partRotat;

	// PARTICLE ROTATION
	auto cur_rotation = rotation->at(lifeTime).get();

	// * YAW ROTATION
	if (cur_rotation[0] != 0)
	{
		partRotat.set(AngleAxisf(cur_rotation[0], Y).matrix());
		rotatedX = partRotat.M * X;
		rotatedZ = partRotat.M * Z;
	}

	// * PITCH ROTATION
	if (cur_rotation[1] != 0)
	{
		Matrix3f rotationX = AngleAxisf(-cur_rotation[1], rotatedX).matrix();
		partRotat.mult(rotationX);
		rotatedZ = rotationX * rotatedZ;
	}

	// * ROLL ROTATION
	if (cur_rotation[2] != 0)
	{
		partRotat.mult(AngleAxisf(cur_rotation[2], rotatedZ).matrix());
	}

	// SYSTEM ROLL ROTATION
	auto cur_direction = rotation_system->getDirection();
	Matrix3fWrapper sysRotat;
	if (cur_direction[1] != 0)
	{
		sysRotat.set(AngleAxisf(cur_direction[1], Z).matrix());
	}

	// PARTICLE OFFSET
	Matrix4fWrapper partOffset;
	if (!description->offset.timeline.empty()) // if not then partOffset, offsetRotat and anti_offsetRotat will not be inited and skipped
	{
		Vector3f cur_offset = offset->at(lifeTime).get();
		if (cur_offset != Vector3f(0, 0, 0))
		{
			// OFFSET ROTATION
			Matrix3fWrapper offsetRotat;
			cur_direction = offset_axis_system->getDirection();

			// * YAW ROTATION
			if (cur_direction[0] != 0)
			{
				offsetRotat.set(AngleAxisf(cur_direction[0], Y).matrix());
				rotatedZ = offsetRotat.M * Z;
			}

			// * ROLL ROTATION
			if (cur_direction[1] != 0)
			{
				offsetRotat.mult(AngleAxisf(cur_direction[1], rotatedZ).matrix());
			}

			if (offsetRotat.valued)
			{
				cur_offset = offsetRotat.M * cur_offset;
			}

			partOffset.set(Affine3f(Translation3f(cur_offset)).matrix());
		}
	}

	// PARTICLE TRANSLATION
	Matrix4fWrapper partTrans;
	Vector3f cur_coords = coords->at(lifeTime).get();
	if (cur_coords != Vector3f(0, 0, 0))
	{
		// TRANSLATION ROTATION
		Matrix3fWrapper transRotat;
		cur_direction = coords_axis_system->getDirection();

		// * YAW ROTATION
		if (cur_direction[0] != 0)
		{
			transRotat.set(AngleAxisf(cur_direction[0], Y).matrix());
			rotatedZ = transRotat.M * Z;
		}

		// * ROLL ROTATION
		if (cur_direction[1] != 0)
		{
			transRotat.mult(AngleAxisf(cur_direction[1], rotatedZ).matrix());
		}

		if (transRotat.valued)
		{
			cur_coords = transRotat.M * cur_coords;
		}
		
		partTrans.set(Affine3f(Translation3f(cur_coords)).matrix());
	}

	// EFFECT GRAVITY
	Vector3f effect_gravity_shift;
	if (params.enable_effect_gravity)
	{
		effect_gravity_shift = params.effect_scaling.block<3, 3>(0, 0).matrix() * (description->gravity * lifeTime * lifeTime / 2); // S = a * t^2 / 2
		if (params.enable_effect_rotation)
		{
			effect_gravity_shift = coords_system->getOuterRotation() * effect_gravity_shift;
		}
	}
	else
	{
		effect_gravity_shift = { 0, 0, 0 };
	}

	// GLOBAL GRAVITY
	Vector3f global_gravity_shift;
	if (params.enable_global_gravity)
	{
		global_gravity_shift = coords_system->getOuterRotation() * params.globalGravity * lifeTime * lifeTime / 2; // S = a * t^2 / 2
	}
	else
	{
		global_gravity_shift = { 0, 0, 0 };
	}

	// SYSTEM TRANSLATION + GRAVITY + INERTION
	Matrix4fWrapper sysTrans;
	Vector3f sys_coords = coords_system->getCoords() + effect_gravity_shift + global_gravity_shift + birth.inertion * lifeTime;
	if (sys_coords != Vector3f(0, 0, 0))
	{
		sysTrans.set(Affine3f(Translation3f(sys_coords)).matrix());
	}

	// A.mult(B) means A = B * A;
	// so here: positioning = sysTrans * transRotat * partTrans * offsetRotat * ...
			// ... * partOffset * anti_offsetRotat * anti_transRotat * sysRotat * partRotat * scaling

	Matrix4fWrapper partRotat4;
	Matrix4fWrapper effectScaling;
	Matrix3fWrapper effectRotat;

	effectRotat.set(coords_system->getOuterRotation());

	if (params.enable_effect_rotation) partRotat.mult(effectRotat);
	partRotat.mult(sysRotat);

	partRotat4.set(partRotat); // convert Matrix3f to Matrix4f
	scaling.mult(partRotat4);

	scaling.mult(partOffset);

	scaling.mult(partTrans);

	Matrix4fWrapper R4;
	R4.set(effectRotat);
	if (!params.enable_effect_rotation) scaling.mult(R4);

	effectScaling.set(params.effect_scaling);
	scaling.mult(effectScaling);

	scaling.mult(sysTrans);

	transformation = scaling.valued ? scaling.M : Matrix4f::Identity();
}

CoordinateSystem* Particle::getEmitterSystem(RelatedCoordinateSystem flag)
{
	switch (flag)
	{
	case RelatedCoordinateSystem::PARTICLE_EFFECT: return root_system;
	default: return this;
	}
}

// TRANSFORM MATRIX WRAPPERS

Matrix3fWrapper::Matrix3fWrapper()
{
	valued = false;
}

void Matrix3fWrapper::set(Matrix3f& M)
{
	this->M = M;
	valued = true;
}

void Matrix3fWrapper::set(Matrix3f&& M)
{
	this->M = M;
	valued = true;
}

void Matrix3fWrapper::mult(Matrix3f& other)
{
	if (!valued)
	{
		set(std::move(other));
		return;
	}

	M = other * M;
}

void Matrix3fWrapper::mult(Matrix3fWrapper& other)
{
	if (!other.valued)
	{
		return;
	}

	if (!valued)
	{
		set(std::move(other.M));
		return;
	}

	M = other.M * M;
}

Matrix4f Matrix3fWrapper::extent()
{
	return Affine3f(M).matrix();
}

Matrix4fWrapper::Matrix4fWrapper()
{
	valued = false;
}

void Matrix4fWrapper::set(Matrix4f& M)
{
	this->M = M;
	valued = true;
}

void Matrix4fWrapper::set(Matrix4f&& M)
{
	this->M = M;
	valued = true;
}

void Matrix4fWrapper::set(Matrix3fWrapper& M)
{
	if (M.valued)
	{
		set(M.extent());
	}
}

void Matrix4fWrapper::mult(Matrix4fWrapper& other)
{
	if (!other.valued)
	{
		return;
	}

	if (!valued)
	{
		set(std::move(other.M));
	}

	M = other.M * M;
}