#include "ParticleManager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{

}

std::shared_ptr<ParticleEffect> ParticleManager::addEffect(std::string& effectPath)
{
	std::shared_ptr<ParticleEffect> effect;

	try
	{
		boost::property_tree::ptree JSONsource;
		boost::property_tree::json_parser::read_json(effectPath, JSONsource);
		effect = std::make_shared<ParticleEffect>();
		effect->parse(JSONsource);
	}
	catch (...)
	{
		return nullptr;
	}

	return effect;
}