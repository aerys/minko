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

#include "minko/math/Vector3.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/Container.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"

using namespace minko::math;
using namespace minko::component;

LightManager::LightManager() :
	AbstractComponent(),
	std::enable_shared_from_this<LightManager>(),
	_data(data::Provider::create()),
	_ambientLights(),
	_directionalLights(),
	_pointLights(),
	_spotLights(),
	_sumAmbientColors(Vector3::create(0.0f, 0.0f, 0.0f)),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_componentAddedSlot(nullptr),
	_componentRemovedSlot(nullptr)
{
	setLightColor("ambientLights.sumColors", _sumAmbientColors);
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

	_sumAmbientColors->copyFrom(Vector3::zero());

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

	auto sumAmbientColors		= Vector3::create()->copyFrom(_sumAmbientColors);
	_sumAmbientColors->copyFrom(Vector3::zero());

	auto descendants = scene::NodeSet::create(target)->descendants(true);
	for (auto& descendant : descendants->nodes())
	{
		for (auto& ambientLight : descendant->components<AmbientLight>())
		{
			_ambientLights.insert(ambientLight);
			_sumAmbientColors += ambientLight->color() * ambientLight->ambient();
		}

		for (auto& directionalLight : descendant->components<DirectionalLight>())
			_directionalLights.insert(directionalLight);

		for (auto& pointLight : descendant->components<PointLight>())
			_pointLights.insert(pointLight);

		for (auto& spotLight : descendant->components<SpotLight>())
			_spotLights.insert(spotLight);
	}

	if (numAmbientLights != _ambientLights.size())
		setLightArrayLength("ambientLights.length", _ambientLights.size());
	if (numDirectionalLights != _directionalLights.size())
		setLightArrayLength("directionalLights.length", _directionalLights.size());
	if (numPointLights != _pointLights.size())
		setLightArrayLength("pointLights.length", _pointLights.size());
	if (numSpotLights != _spotLights.size())
		setLightArrayLength("spotLights.length", _spotLights.size());

	if ((sumAmbientColors - _sumAmbientColors)->length() > 1e-6f)
		setLightColor("ambientLights.sumColors", _sumAmbientColors);
}

void
LightManager::removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
{
	auto numAmbientLights		= _ambientLights.size();
	auto numDirectionalLights	= _directionalLights.size();
	auto numPointLights			= _pointLights.size();
	auto numSpotLights			= _spotLights.size();

	auto sumAmbientColors		= Vector3::create()->copyFrom(_sumAmbientColors);
	_sumAmbientColors->copyFrom(Vector3::zero());

	auto descendants = scene::NodeSet::create(target)->descendants(true);
	for (auto& descendant : descendants->nodes())
	{
		for (auto& ambientLight : descendant->components<AmbientLight>())
		{
			_ambientLights.erase(ambientLight);
			_sumAmbientColors += ambientLight->color() * ambientLight->ambient();
		}

		for (auto& directionalLight : descendant->components<DirectionalLight>())
			_directionalLights.erase(directionalLight);

		for (auto& pointLight : descendant->components<PointLight>())
			_pointLights.erase(pointLight);

		for (auto& spotLight : descendant->components<SpotLight>())
			_spotLights.erase(spotLight);
	}

	if (numAmbientLights != _ambientLights.size())
		setLightArrayLength("ambientLights.length", _ambientLights.size());
	if (numDirectionalLights != _directionalLights.size())
		setLightArrayLength("directionalLights.length", _directionalLights.size());
	if (numPointLights != _pointLights.size())
		setLightArrayLength("pointLights.length", _pointLights.size());
	if (numSpotLights != _spotLights.size())
		setLightArrayLength("spotLights.length", _spotLights.size());

	if ((sumAmbientColors - _sumAmbientColors)->length() > 1e-6f)
		setLightColor("ambientLights.sumColors", _sumAmbientColors);
}

void
LightManager::componentAddedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp)
{
	auto ambientLight		= std::dynamic_pointer_cast<AmbientLight>(cmp);
	auto directionalLight	= std::dynamic_pointer_cast<DirectionalLight>(cmp);
	auto pointLight			= std::dynamic_pointer_cast<PointLight>(cmp);
	auto spotLight			= std::dynamic_pointer_cast<SpotLight>(cmp);

	if (ambientLight && _ambientLights.insert(ambientLight).second)
		setLightArrayLength("ambientLights.length", _ambientLights.size());
	else if (directionalLight && _directionalLights.insert(directionalLight).second)
		setLightArrayLength("directionalLights.length", _directionalLights.size());
	else if (pointLight && _pointLights.insert(pointLight).second)
		setLightArrayLength("pointLights.length", _pointLights.size());
	else if (spotLight && _spotLights.insert(spotLight).second)
		setLightArrayLength("spotLights.length", _spotLights.size());
}

void
LightManager::componentRemovedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp)
{
	auto ambientLight		= std::dynamic_pointer_cast<AmbientLight>(cmp);
	auto directionalLight	= std::dynamic_pointer_cast<DirectionalLight>(cmp);
	auto pointLight			= std::dynamic_pointer_cast<PointLight>(cmp);
	auto spotLight			= std::dynamic_pointer_cast<SpotLight>(cmp);

	if (ambientLight && _ambientLights.erase(ambientLight))
		setLightArrayLength("ambientLights.length", _ambientLights.size());
	else if (directionalLight && _directionalLights.erase(directionalLight))
		setLightArrayLength("directionalLights.length", _directionalLights.size());
	else if (pointLight && _pointLights.insert(pointLight).second)
		setLightArrayLength("pointLights.length", _pointLights.size());
	else if (spotLight && _spotLights.insert(spotLight).second)
		setLightArrayLength("spotLights.length", _spotLights.size());
}

void
LightManager::setLightColor(const std::string& arrayName, Vector3::Ptr color)
{
	_data->set<Vector3>(arrayName, *color);
}

void
LightManager::setLightArrayLength(const std::string& arrayName, int length)
{
	if (length == 0)
	{
		if (_data->hasProperty(arrayName))
			_data->unset(arrayName);
	}
	else
		_data->set<int>(arrayName, length);
}
