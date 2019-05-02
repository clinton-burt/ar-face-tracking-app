#ifndef PARTICLES_MANAGER_H
#define PARTICLES_MANAGER_H

#include <map>
#include <string>
#include <memory>

#include <particles\Particles.h>

class ParticleManager
{
public:

	ParticleManager();
	~ParticleManager();

	std::shared_ptr<ParticleEffect> addEffect(std::string& path);
};

#endif PARTICLES_MANAGER_H