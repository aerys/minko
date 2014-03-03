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

#include "minko/component/ParticleSystem.hpp"

#include "minko/file/AssetLibrary.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/data/ParticlesProvider.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/ParticleVertexBuffer.hpp"
#include "minko/render/ParticleIndexBuffer.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/particle/ParticleData.hpp"
#include "minko/particle/StartDirection.hpp"
#include "minko/particle/modifier/IParticleModifier.hpp"
#include "minko/particle/modifier/IParticleInitializer.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"
#include "minko/particle/shape/Sphere.hpp"
#include "minko/particle/sampler/Sampler.hpp"
#include "minko/particle/tools/VertexComponentFlags.hpp"


using namespace minko;
using namespace minko::component;
using namespace minko::particle;

#define EPSILON 0.001

ParticleSystem::ParticleSystem(AbstractContextPtr	context,
							   AssetLibraryPtr		assets,
							   float				rate,
							   FloatSamplerPtr		lifetime,
							   ShapePtr				shape,
							   StartDirection		startDirection,
							   FloatSamplerPtr		startVelocity): 
	_countLimit			(16384),
	_maxCount			(0),
	_liveCount			(0),
	_previousLiveCount	(0),
	_particles			(),
	_isInWorldSpace		(false),
	_isZSorted			(false),
	_useOldPosition		(false),
	_rate				(1 / rate),
	_lifetime			(lifetime),
	_shape				(shape),
	_startDirection		(startDirection),
	_startVelocity		(startVelocity),
	_createTimer		(0),
	_format				(VertexComponentFlags::DEFAULT),
	_updateStep			(0),
	_playing			(false),
	_emitting			(true),
	_time				(0)
{
	_geometry	= geometry::ParticlesGeometry::create(context);
	_material	= data::ParticlesProvider::create();
	_effect		= assets->effect("particles");

	if (_effect == nullptr)
		throw new std::logic_error("Effect 'particles' is not available in the asset library.");

	_surface	= Surface::create(_geometry, _material, _effect);

	if (_shape == nullptr)
		_shape	= shape::Sphere::create(10);

	_comparisonObject.system = (this);

	updateMaxParticlesCount();
}

void
ParticleSystem::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&ParticleSystem::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&ParticleSystem::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
ParticleSystem::targetAddedHandler(AbsCompPtr	ctrl,
								   NodePtr 		target)
{
	findSceneManager();

	target->addComponent(_surface);

	auto nodeCallback = [&](NodePtr, NodePtr, NodePtr) { findSceneManager(); };

	_addedSlot = target->added()->connect(nodeCallback);
	_removedSlot = target->removed()->connect(nodeCallback);

	auto componentCallback = [&](NodePtr, NodePtr, AbsCompPtr) { findSceneManager(); };

	_componentAddedSlot = target->root()->componentAdded()->connect(componentCallback);
	_componentRemovedSlot = target->root()->componentRemoved()->connect(componentCallback);
}

void
ParticleSystem::targetRemovedHandler(AbsCompPtr ctrl,
									 NodePtr	target)
{
	findSceneManager();

	target->removeComponent(_surface);
	
	_addedSlot = nullptr;
	_removedSlot = nullptr;
	_componentAddedSlot = nullptr;
	_componentRemovedSlot = nullptr;
}

void
ParticleSystem::findSceneManager()
{
	NodeSetPtr roots = scene::NodeSet::create(targets())
		->roots()
		->where([](NodePtr node)
		{
			return node->hasComponent<SceneManager>();
		});

	if (roots->nodes().size() > 1)
		throw std::logic_error("ParticleSystem cannot be in two separate scenes.");
	else if (roots->nodes().size() == 1)
		_frameEndSlot = roots->nodes()[0]->component<SceneManager>()->frameEnd()->connect(std::bind(
			&ParticleSystem::frameEndHandler, 
            shared_from_this(), 
            std::placeholders::_1
		));
	else
		_frameEndSlot = nullptr;
}

void
ParticleSystem::frameEndHandler(SceneManager::Ptr sceneManager)
{	
	if (!_playing)
		return;

	if (_isInWorldSpace)
		_toWorld = targets()[0]->components<Transform>()[0];

	clock_t now	= clock();
	float deltaT = (float)(now - _previousClock) / CLOCKS_PER_SEC;
	_previousClock = now;

	if (_updateStep == 0)
	{
			updateSystem(deltaT, _emitting);
			updateVertexBuffer();
	}
	else
	{
		bool changed = false;

		_time += deltaT;

		while (_time > _updateStep)
		{
			updateSystem(_updateStep, _emitting);
			changed = true;
			_time -= _updateStep;
		}
		if (changed)
			updateVertexBuffer();
	}
}

ParticleSystem::Ptr
ParticleSystem::add(ModifierPtr	modifier)
{
	addComponents(modifier->getNeededComponents());

	modifier->setProperties(_material);
	
	IInitializerPtr i = std::dynamic_pointer_cast<modifier::IParticleInitializer> (modifier);

	if (i != 0)
	{
		_initializers.push_back(i);

		return shared_from_this();	
	}

	IUpdaterPtr u = std::dynamic_pointer_cast<modifier::IParticleUpdater> (modifier);

	if (u != 0)
		_updaters.push_back(u);

    return shared_from_this();	
}

ParticleSystem::Ptr
ParticleSystem::remove(ModifierPtr	modifier)
{
	IInitializerPtr i = std::dynamic_pointer_cast<modifier::IParticleInitializer> (modifier);

	if (i != 0)
	{
		for (auto it = _initializers.begin(); it != _initializers.end(); ++it)
		{
			if (*it == i)
			{
				_initializers.erase(it);
				modifier->unsetProperties(_material);
				updateVertexFormat();

				return shared_from_this();
			}
		}

		return shared_from_this();
	}
	
	IUpdaterPtr u = std::dynamic_pointer_cast<modifier::IParticleUpdater> (modifier);

	if (u != 0)
	{
		for (auto it = _updaters.begin(); it != _updaters.end(); ++it)
		{
			if (*it == u)
			{
				_updaters.erase(it);
				modifier->unsetProperties(_material);
				updateVertexFormat();

				return shared_from_this();
			}
		}
	}

    return shared_from_this();
}

bool
ParticleSystem::has(ModifierPtr modifier) const
{
	IInitializerPtr i = std::dynamic_pointer_cast<modifier::IParticleInitializer> (modifier);

	if (i != 0)
	{
		for (auto it = _initializers.begin();
			it != _initializers.end();
			++it)
		{
			if (*it == i)
			{
				return true;
			}
		}

		return false;
	}
	
	IUpdaterPtr u = std::dynamic_pointer_cast<modifier::IParticleUpdater> (modifier);

	if (u != 0)
	{
		for (auto it = _updaters.begin(); it != _updaters.end(); ++it)
		{
			if (*it == u)
			{
				return true;
			}
		}

		return false;
	}

	return false;
}

void
ParticleSystem::fastForward(float time, unsigned int updatesPerSecond)
{
	float updateStep = _updateStep;

	if (updatesPerSecond != 0)
		updateStep = 1.f / updatesPerSecond;

	while(time > updateStep)
	{
		updateSystem(updateStep, _emitting);
		time -= updateStep;
	}
}

void
ParticleSystem::updateSystem(float	timeStep, bool emit)
{
	if (emit && _createTimer < _rate)
		_createTimer += timeStep;

	for (unsigned particleIndex = 0; particleIndex < _particles.size(); ++particleIndex)
	{
		ParticleData& particle = _particles[particleIndex];

		if (particle.alive)
		{
			particle.timeLived += timeStep;

			particle.oldx = particle.x;
			particle.oldy = particle.y;
			particle.oldz = particle.z;

			if (particle.timeLived >= particle.lifetime)
				killParticle(particleIndex);
		}
	}

	if (_format & VertexComponentFlags::OLD_POSITION)
	{
		for (unsigned particleIndex = 0; particleIndex < _particles.size(); ++particleIndex)
		{
			ParticleData& particle = _particles[particleIndex];

			particle.oldx = particle.x;
			particle.oldy = particle.y;
			particle.oldz = particle.z;
		}
	}
	
	for (unsigned int i = 0; i < _updaters.size(); ++i)
		_updaters[i]->update(_particles, timeStep);

	for (unsigned particleIndex = 0; particleIndex < _particles.size(); ++particleIndex)
	{
		ParticleData& particle = _particles[particleIndex];
		
		if (!particle.alive && emit && _createTimer >= _rate)
		{
			_createTimer -= _rate;
			createParticle(particleIndex, *_shape, _createTimer);
			particle.lifetime = _lifetime->value();
		}
		
		particle.rotation += particle.startAngularVelocity * timeStep;

		particle.startvx += particle.startfx * timeStep;
		particle.startvy += particle.startfy * timeStep;
		particle.startvz += particle.startfz * timeStep;

		particle.x += particle.startvx * timeStep;
		particle.y += particle.startvy * timeStep;
		particle.z += particle.startvz * timeStep;
	}
}

void
ParticleSystem::createParticle(unsigned int 				particleIndex,
							   const shape::EmitterShape&	shape,
							   float						timeLived)
{
	ParticleData& particle = _particles[particleIndex];

	if (_startDirection == StartDirection::NONE)
	{
		shape.initPosition(particle);

		particle.startvx 	= 0;
		particle.startvy 	= 0;
		particle.startvz 	= 0;
	}
	else if (_startDirection == StartDirection::SHAPE)
	{
		shape.initPositionAndDirection(particle);
	}
	else if (_startDirection == StartDirection::RANDOM)
	{
		shape.initPosition(particle);
	}
	else if (_startDirection == StartDirection::UP)
	{
		shape.initPosition(particle);


		particle.startvx 	= 0;
		particle.startvy 	= 1;
		particle.startvz 	= 0;
	}
	else if (_startDirection == StartDirection::OUTWARD)
	{
		shape.initPosition(particle);


		particle.startvx 	= particle.x;
		particle.startvy 	= particle.y;
		particle.startvz 	= particle.z;
	}

	particle.oldx 	= particle.x;
	particle.oldy 	= particle.y;
	particle.oldz 	= particle.z;

	if (_isInWorldSpace)
	{
		const std::vector<float>& transform = _toWorld->matrix()->data();

		float x = particle.x;
		float y = particle.y;
		float z = particle.z;

		particle.x = transform[0] * x + transform[1] * y + transform[2] * z + transform[3];
		particle.y = transform[4] * x + transform[5] * y + transform[6] * z + transform[7];
		particle.z = transform[8] * x + transform[9] * y + transform[10] * z + transform[11];

		if (_startDirection != StartDirection::NONE)
		{
			float vx = particle.startvx;
			float vy = particle.startvy;
			float vz = particle.startvz;

			particle.startvx = transform[0] * vx + transform[1] * vy + transform[2] * vz;
			particle.startvy = transform[4] * vx + transform[5] * vy + transform[6] * vz;
			particle.startvz = transform[8] * vx + transform[9] * vy + transform[10] * vz;
		}
	}

	if (_startDirection != StartDirection::NONE)
	{
		float norm = sqrt(particle.startvx * particle.startvx + 
						  particle.startvy * particle.startvy + 
						  particle.startvz * particle.startvz);

		float v = _startVelocity ? _startVelocity->value() : 1;

		particle.startvx 	= particle.startvx / norm * v;
		particle.startvy 	= particle.startvy / norm * v;
		particle.startvz 	= particle.startvz / norm * v;
	}

	particle.rotation 				= 0;
	particle.startAngularVelocity 	= 0;

	particle.timeLived	= timeLived;
			
	particle.alive 		= true;
	
	++_liveCount;

	for (unsigned int i = 0; i < _initializers.size(); ++i)
		_initializers[i]->initialize(particle, timeLived);
}

void
ParticleSystem::killParticle(unsigned int	particleIndex)
{
	_particles[particleIndex].alive = false;
	
	--_liveCount;
}

void
ParticleSystem::updateMaxParticlesCount()
{
	auto value = (unsigned int)(ceilf(_lifetime->max() / _rate - (float)EPSILON));
	value = value > _countLimit ? _countLimit : value;
	
	if (_maxCount == value)
		return;

	_maxCount = value;

	_liveCount = 0;
	for (unsigned int i = 0; i < _particles.size(); ++i)
	{
		if (_particles[i].alive)
		{
			if (_liveCount == _maxCount
				|| _particles[i].timeLived >= _lifetime->max())
				_particles[i].alive = false;
			else
			{
				++_liveCount;
				if (_particles[i].lifetime > _lifetime->max() || _particles[i].lifetime < _lifetime->min())
					_particles[i].lifetime = _lifetime->value();
			}
		}
	}
	resizeParticlesVector();
	_geometry->initStreams(_maxCount);
}

void
ParticleSystem::resizeParticlesVector()
{
	_particles.resize(_maxCount);
	if (_isZSorted)
	{
		_particleDistanceToCamera.resize(_maxCount);
		_particleOrder.resize(_maxCount);
		for (unsigned int i = 0; i < _particleOrder.size(); ++i)
			_particleOrder[i] = i;
	}
	else
	{
		_particleDistanceToCamera.resize(0);
		_particleOrder.resize(0);
	}
}

void
ParticleSystem::updateParticleDistancesToCamera()
{
	for (unsigned int i = 0; i < _particleDistanceToCamera.size(); ++i)
	{
		const ParticleData& particle = _particles[i];

		float x = particle.x;
		float y = particle.y;
		float z = particle.z;
		
		if (!_isInWorldSpace)
		{
			x = _localToWorld[0] * x + _localToWorld[4] * y + _localToWorld[8] * z + _localToWorld[12];
			y = _localToWorld[1] * x + _localToWorld[5] * y + _localToWorld[9] * z + _localToWorld[13];
			z = _localToWorld[2] * x + _localToWorld[6] * y + _localToWorld[10] * z + _localToWorld[14];
		}

		float deltaX = _cameraCoords[0] - x;
		float deltaY = _cameraCoords[1] - y;
		float deltaZ = _cameraCoords[2] - z;

		_particleDistanceToCamera[i] = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
	}
}

void
ParticleSystem::reset()
{
	if(_liveCount == 0)
		return;

	_liveCount = 0;

	for (unsigned particleIndex = 0; particleIndex < _particles.size(); ++particleIndex)
	{
		_particles[particleIndex].alive = false;
	}
}


void
ParticleSystem::addComponents(unsigned int components, bool blockVSInit)
{
    typedef std::tuple<std::string, VertexComponentFlags, unsigned int> ComponentInfo;
    static const std::array<ComponentInfo, 6> OPTIONAL_COMPONENTS = 
    {
        std::make_tuple("size",          VertexComponentFlags::SIZE,            1),
        std::make_tuple("color",         VertexComponentFlags::COLOR,           3),
        std::make_tuple("time",          VertexComponentFlags::TIME,            1),
        std::make_tuple("oldPosition",   VertexComponentFlags::OLD_POSITION,    3),
        std::make_tuple("rotation",      VertexComponentFlags::ROTATION,        1),
        std::make_tuple("spriteIndex",   VertexComponentFlags::SPRITE_INDEX,    1)
    };
    
	if (_format == components)
		return;

	_format |= components;

    // FIXME: should be made fully dynamic
	auto vertexBuffer = _geometry->particleVertices();

    _geometry->removeVertexBuffer(vertexBuffer);
    for (auto& component : OPTIONAL_COMPONENTS)
    {
        const auto& attrName = std::get<0>(component);

        if (vertexBuffer->hasAttribute(attrName))
            vertexBuffer->removeAttribute(attrName); // attribute offset must be updated
    }

    // mandatory vertex attributes: offset and position
    assert(vertexBuffer->hasAttribute("offset") && vertexBuffer->hasAttribute("position"));
    unsigned int attrOffset = 5;

    for (auto& component : OPTIONAL_COMPONENTS)
    {
        const auto& attrName    = std::get<0>(component);
        const auto  attrFlag    = std::get<1>(component);
        const auto  attrSize    = std::get<2>(component);

        if ((_format & attrFlag))
        {
            vertexBuffer->addAttribute(attrName, attrSize, attrOffset);
            attrOffset += attrSize;
        }
    }

    _geometry->addVertexBuffer(vertexBuffer);

	if (!blockVSInit)
		_geometry->initStreams(_maxCount);
}

unsigned int
ParticleSystem::updateVertexFormat()
{
	_format = VertexComponentFlags::DEFAULT;
	
	/*
	auto vb = _geometry->vertices();
	if (!vb->hasAttribute("offset"))
		vb->addAttribute("offset", 2, 0);
	if (!vb->hasAttribute("position"))
		vb->addAttribute("position", 3, 2);
	*/

	for (auto it = _initializers.begin();
		 it != _initializers.end();
		 ++it)
	{
		addComponents((*it)->getNeededComponents(), true);
	}

	for (auto it = _updaters.begin();
		 it != _updaters.end();
		 ++it)
	{
		addComponents((*it)->getNeededComponents(), true);
	}

	if (_useOldPosition)
		addComponents(VertexComponentFlags::OLD_POSITION, true);
	
	_geometry->initStreams(_maxCount);

	return _format;
}

void
ParticleSystem::updateVertexBuffer()
{
	if (_liveCount == 0)
		return;

	if (_isZSorted)
	{
		updateParticleDistancesToCamera();
		std::sort(_particleOrder.begin(), _particleOrder.end(), _comparisonObject);
	}
	
	std::vector<float>& vsData = _geometry->particleVertices()->data();
	float* vertexIterator	= &(*vsData.begin());

	for (unsigned int particleIndex = 0; particleIndex < _maxCount; ++particleIndex)
	{
		ParticleData* particle;

		if (_isZSorted)
			particle = &_particles[_particleOrder[particleIndex]];
		else
			particle = &_particles[particleIndex];

		unsigned int i = 5;

		if (particle->alive)
		{
			setInVertexBuffer(vertexIterator, 2, particle->x);
			setInVertexBuffer(vertexIterator, 3, particle->y);
			setInVertexBuffer(vertexIterator, 4, particle->z);

			if (_format & VertexComponentFlags::SIZE)
				setInVertexBuffer(vertexIterator, i++, particle->size);

			if (_format & VertexComponentFlags::COLOR)
			{
				setInVertexBuffer(vertexIterator, i++, particle->r);
				setInVertexBuffer(vertexIterator, i++, particle->g);
				setInVertexBuffer(vertexIterator, i++, particle->b);
			}

			if (_format & VertexComponentFlags::TIME)
				setInVertexBuffer(vertexIterator, i++, particle->timeLived / particle->lifetime);

			if (_format & VertexComponentFlags::OLD_POSITION)
			{
				setInVertexBuffer(vertexIterator, i++, particle->oldx);
				setInVertexBuffer(vertexIterator, i++, particle->oldy);
				setInVertexBuffer(vertexIterator, i++, particle->oldz);
			}

			if (_format & VertexComponentFlags::ROTATION)
				setInVertexBuffer(vertexIterator, i++, particle->rotation);

			if (_format & VertexComponentFlags::SPRITE_INDEX)
				setInVertexBuffer(vertexIterator, i++, particle->spriteIndex);

			vertexIterator += 4 * _geometry->vertexSize();
		}
	}

    _geometry->particleVertices()->upload(0, _liveCount << 2);

	if (_liveCount != _previousLiveCount)
	{
        auto particleIndices    = std::static_pointer_cast<render::ParticleIndexBuffer>(_geometry->indices());
        particleIndices->upload(0, _liveCount << 2);
		_previousLiveCount      = _liveCount;
	}
}

ParticleSystem::Ptr
ParticleSystem::isInWorldSpace(bool value)
{
    _isInWorldSpace = value;

    _material->isInWorldSpace(value);

    return std::static_pointer_cast<ParticleSystem>(shared_from_this());
}

ParticleSystem::Ptr
ParticleSystem::isZSorted(bool value)
{
	_isZSorted = value;

	resizeParticlesVector();

    return std::static_pointer_cast<ParticleSystem>(shared_from_this());
};

ParticleSystem::Ptr
ParticleSystem::useOldPosition(bool value)
{
	if (value != _useOldPosition)
    {
	    _useOldPosition = value;
	    updateVertexFormat();
    }

    return std::static_pointer_cast<ParticleSystem>(shared_from_this());
};