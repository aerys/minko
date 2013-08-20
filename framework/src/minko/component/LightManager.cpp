/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/component/LightManager.hpp"

#include "minko/data/Provider.hpp"
#include "minko/data/Container.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AbstractLight.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"

using namespace minko::math;
using namespace minko::data;
using namespace minko::component;

LightManager::LightManager() :
	AbstractComponent(),
	std::enable_shared_from_this<LightManager>(),
	_data(Provider::create()),
	_sumAmbients(Vector3::create(0.0f, 0.0f, 0.0f)),
	_ambientLights(),
	_directionalLights(),
	_pointLights(),
	_spotLights(),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_componentAddedSlot(nullptr),
	_componentRemovedSlot(nullptr)
{
	updateSumAmbients(_sumAmbients);
}

void
LightManager::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&LightManager::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&LightManager::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
LightManager::targetAddedHandler(AbsCmpPtr cmp, NodePtr target)
{
	if (target->components<LightManager>().size() > 1)
		throw std::logic_error("There cannot be more than one LightManager.");
	if (target->root() != target)
		throw std::invalid_argument("target");

	_addedSlot = target->added()->connect(std::bind(
		&LightManager::addedHandler,
		std::dynamic_pointer_cast<LightManager>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed()->connect(std::bind(
		&LightManager::removedHandler,
		std::dynamic_pointer_cast<LightManager>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentAddedSlot = target->componentAdded()->connect(std::bind(
		&LightManager::componentAddedHandler,
		std::dynamic_pointer_cast<LightManager>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_componentRemovedSlot = target->componentRemoved()->connect(std::bind(
		&LightManager::componentRemovedHandler,
		std::dynamic_pointer_cast<LightManager>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	target->data()->addProvider(_data);

	addedHandler(nullptr, target, nullptr);
}

void
LightManager::targetRemovedHandler(AbsCmpPtr cmp, NodePtr target)
{
	_ambientLights.clear();
	_directionalLights.clear();
	_pointLights.clear();
	_spotLights.clear();
	_addedSlot				= nullptr;
	_removedSlot			= nullptr;
	_componentAddedSlot		= nullptr;
	_componentRemovedSlot	= nullptr;

	target->data()->removeProvider(_data);
}

void
LightManager::addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	auto numAmbientLights		= _ambientLights.size();
	auto numDirectionalLights	= _directionalLights.size();
	auto numPointLights			= _pointLights.size();
	auto numSpotLights			= _spotLights.size();
	std::vector<AbstractLight::Ptr> lights;

	auto descendants			= scene::NodeSet::create(target)->descendants(true);
	auto ambientIncrease		= Vector3::create(0.0f, 0.0f, 0.0f); 
	for (auto& descendant : descendants->nodes())
	{
		for (auto& ambientLight : descendant->components<AmbientLight>())
		{
			if (addLight(ambientLight, _ambientLights))
			{
				lights.push_back(ambientLight);
				ambientIncrease += ambientLight->color() * ambientLight->ambient();
			}
		}

		for (auto& directionalLight : descendant->components<DirectionalLight>())
			if (addLight(directionalLight, _directionalLights))
				lights.push_back(directionalLight);

		for (auto& pointLight : descendant->components<PointLight>())
			if (addLight(pointLight, _pointLights))
				lights.push_back(pointLight);

		for (auto& spotLight : descendant->components<SpotLight>())
			if (addLight(spotLight, _spotLights))
				lights.push_back(spotLight);
	}

	if (numAmbientLights != _ambientLights.size())
		updateLightArray("ambientLights", _ambientLights);
	if (fabsf(ambientIncrease->length()) > 1e-3f)
		updateSumAmbients(_sumAmbients + ambientIncrease);
	if (numDirectionalLights != _directionalLights.size())
		updateLightArray("directionalLights", _directionalLights);
	if (numPointLights != _pointLights.size())
		updateLightArray("pointLights", _pointLights);
	if (numSpotLights != _spotLights.size())
		updateLightArray("spotLights", _spotLights);

	for (auto light : lights)
		targets()[0]->data()->addProvider(light->_arrayData);
}

void
LightManager::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	auto numAmbientLights		= _ambientLights.size();
	auto numDirectionalLights	= _directionalLights.size();
	auto numPointLights			= _pointLights.size();
	auto numSpotLights			= _spotLights.size();
	std::vector<AbstractLight::Ptr> lights;

	auto descendants			= scene::NodeSet::create(target)->descendants(true);
	auto ambientDecrease		= Vector3::create(0.0f, 0.0f, 0.0f); 
	for (auto& descendant : descendants->nodes())
	{
		for (auto& ambientLight : descendant->components<AmbientLight>())
			if (removeLight(ambientLight, _ambientLights))
			{
				lights.push_back(ambientLight);
				ambientDecrease += ambientLight->color() * ambientLight->ambient();
			}
	
		for (auto& directionalLight : descendant->components<DirectionalLight>())
			if (removeLight(directionalLight, _directionalLights))
				lights.push_back(directionalLight);

		for (auto& pointLight : descendant->components<PointLight>())
			if (removeLight(pointLight, _pointLights))
				lights.push_back(pointLight);

		for (auto& spotLight : descendant->components<SpotLight>())
			if (removeLight(spotLight, _spotLights))
				lights.push_back(spotLight);
	}

	for (auto light : lights)
		targets()[0]->data()->removeProvider(light->_arrayData);

	if (numAmbientLights != _ambientLights.size())
		updateLightArray("ambientLights", _ambientLights);
	if (fabsf(ambientDecrease->length()) > 1e-3f)
		updateSumAmbients(_sumAmbients - ambientDecrease);
	if (numDirectionalLights != _directionalLights.size())
		updateLightArray("directionalLights", _directionalLights);
	if (numPointLights != _pointLights.size())
		updateLightArray("pointLights", _pointLights);
	if (numSpotLights != _spotLights.size())
		updateLightArray("spotLights", _spotLights);
}

void
LightManager::componentAddedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp)
{
	auto ambientLight		= std::dynamic_pointer_cast<AmbientLight>(cmp);
	auto directionalLight	= std::dynamic_pointer_cast<DirectionalLight>(cmp);
	auto pointLight			= std::dynamic_pointer_cast<PointLight>(cmp);
	auto spotLight			= std::dynamic_pointer_cast<SpotLight>(cmp);

	if (ambientLight && addLight(ambientLight, _ambientLights))
	{
		updateLightArray("ambientLights", _ambientLights);
		targets()[0]->data()->addProvider(ambientLight->_arrayData);
	}
	else if (directionalLight && addLight(directionalLight, _directionalLights))
	{
		updateLightArray("directionalLights", _directionalLights);
		targets()[0]->data()->addProvider(directionalLight->_arrayData);
	}
	else if (pointLight && addLight(pointLight, _pointLights))
	{
		updateLightArray("pointLights", _pointLights);
		targets()[0]->data()->addProvider(pointLight->_arrayData);
	}
	else if (spotLight && addLight(spotLight, _spotLights))
	{
		updateLightArray("spotLights", _spotLights);
		targets()[0]->data()->addProvider(spotLight->_arrayData);
	}
}

void
LightManager::componentRemovedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp)
{
	auto ambientLight		= std::dynamic_pointer_cast<AmbientLight>(cmp);
	auto directionalLight	= std::dynamic_pointer_cast<DirectionalLight>(cmp);
	auto pointLight			= std::dynamic_pointer_cast<PointLight>(cmp);
	auto spotLight			= std::dynamic_pointer_cast<SpotLight>(cmp);

	if (ambientLight && removeLight(ambientLight, _ambientLights))
	{
		targets()[0]->data()->removeProvider(ambientLight->_arrayData);
		updateLightArray("ambientLights", _ambientLights);
	}
	else if (directionalLight && removeLight(directionalLight, _directionalLights))
	{
		targets()[0]->data()->removeProvider(directionalLight->_arrayData);
		updateLightArray("directionalLights", _directionalLights);
	}
	else if (pointLight && removeLight(pointLight, _pointLights))
	{
		targets()[0]->data()->removeProvider(pointLight->_arrayData);
		updateLightArray("pointLights", _pointLights);
	}
	else if (spotLight && removeLight(spotLight, _spotLights))
	{
		targets()[0]->data()->removeProvider(spotLight->_arrayData);
		updateLightArray("spotLights", _spotLights);
	}
}

bool
LightManager::addLight(AbsLightPtr light, std::list<AbsLightPtr>& list)
{
	if (std::find(list.begin(), list.end(), light) != list.end())
		return false;

	list.push_back(light);

	return true;
}

bool
LightManager::removeLight(AbsLightPtr light, std::list<AbsLightPtr>& list)
{
	auto lightIt = std::find(list.begin(), list.end(), light);

	if (lightIt == list.end())
		return false;

	list.erase(lightIt);

	return true;
}

void
LightManager::updateLightArray(const std::string& arrayName, std::list<AbstractLight::Ptr>& lights)
{
	auto propertyName = arrayName + ".length";
	auto length = lights.size();

	if (length == 0)
	{
		if (_data->hasProperty(propertyName))
			_data->unset(propertyName);
	}
	else
		_data->set<int>(propertyName, length);

	auto counter = 0;
	for (auto& light : lights)
		light->lightId(counter++);
}

void
LightManager::updateSumAmbients(Vector3::Ptr value)
{
	_sumAmbients = value;
	_data->set<Vector3>("sumAmbients", *_sumAmbients);
}