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

#include "ParticleSystem.hpp"

#include "minko/AssetsLibrary.hpp"

#include "minko/controller/RenderingController.hpp"
#include "minko/controller/Surface.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/VertexStream.hpp"
#include "minko/render/ParticleIndexStream.hpp"

#include "minko/particle/ParticleData.hpp"
#include "minko/particle/StartDirection.hpp"

#include "minko/particle/modifier/IParticleModifier.hpp"
#include "minko/particle/modifier/IParticleInitializer.hpp"
#include "minko/particle/modifier/IParticleUpdater.hpp"

#include "minko/particle/shape/Sphere.hpp"

#include "minko/particle/sampler/Sampler.hpp"

#include "minko/particle/tools/VertexComponentFlags.hpp"

#include "minko/math/Matrix4x4.hpp"

using namespace minko::controller;
using namespace minko::particle;

#define EPSILON 0.001

ParticleSystem::ParticleSystem(AbstractContextPtr	context,
							   AssetsLibraryPtr		assets,
							   float				rate,
							   FloatSamplerPtr		lifetime,
							   ShapePtr				shape,
							   StartDirection		startDirection,
							   FloatSamplerPtr		startVelocity)
	: _renderers			(scene::NodeSet::create(scene::NodeSet::Mode::AUTO)),
	  _countLimit			(16000),
	  _maxCount				(0),
	  _liveCount			(0),
	  _previousLiveCount	(0),
	  _particles			(),
	  _isInWorldSpace		(false),
	  _isZSorted			(false),
	  _useOldPosition		(false),
	  _rate					(1 / rate),
	  _lifetime				(lifetime),
	  _shape				(shape),
	  _startDirection		(startDirection),
	  _startVelocity		(startVelocity),
	  _createTimer			(0),
	  _format				(VertexComponentFlags::DEFAULT)
{
	_geometry = geometry::ParticlesGeometry::create(context);
	_material = data::Provider::create();
	
	auto view = math::Matrix4x4::create()->perspective(.785f, 800.f / 600.f, .1f, 1000.f);

	_material->set("material/blending",		render::Blending::Mode::ADDITIVE)
		->set("material/depthFunc",	render::CompareMode::ALWAYS)
			 ->set("transform/worldToScreenMatrix",	view);
	

	_effect = assets->effect("particles");

	_surface = Surface::create(_geometry, 
							   _material,
							   _effect);

	if (_shape == 0)
		_shape = shape::Sphere::create(10);

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
	
	_renderers->root()
		->descendants(true)
		->hasController<RenderingController>();
	_rendererAddedSlot = _renderers->nodeAdded()->connect(std::bind(
		&ParticleSystem::rendererAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
	_rendererRemovedSlot = _renderers->nodeRemoved()->connect(std::bind(
		&ParticleSystem::rendererRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
ParticleSystem::targetAddedHandler(AbsCtrlPtr	ctrl,
								   NodePtr 		target)
{	
	target->addController(_surface);
	_renderers->select(targets().begin(), targets().end())->update();
}

void
ParticleSystem::targetRemovedHandler(AbsCtrlPtr ctrl,
									 NodePtr	target)
{
	target->removeController(_surface);
	_renderers->select(targets().begin(), targets().end())->update();
}


void
ParticleSystem::rendererAddedHandler(NodeSetPtr	renderers,
									 NodePtr	rendererNode)
{
	for (auto renderer : rendererNode->controllers<RenderingController>())
	{
		if (_enterFrameSlots.find(renderer) == _enterFrameSlots.end())
		{
			_enterFrameSlots[renderer] = renderer->enterFrame()->connect(std::bind(
				&ParticleSystem::enterFrameHandler,
				shared_from_this(),
				std::placeholders::_1));
		}
	}
}

void
ParticleSystem::rendererRemovedHandler(NodeSetPtr	renderers,
									   NodePtr		rendererNode)
{
	for (auto renderer : rendererNode->controllers<RenderingController>())
	{
		_enterFrameSlots.erase(renderer); 
	}
}


void
ParticleSystem::enterFrameHandler(RenderingCtrlPtr renderer)
{	
	static clock_t  previous = clock();
	static float	time = 0;

	clock_t now	= clock();
	float deltaT = (float)(clock() - previous) / CLOCKS_PER_SEC;
	
	bool changed = false;

	previous = now;
	time += deltaT;

	while (time > .016)
	{
		updateSystem(.016, true);
		changed = true;
		time -= .016;
	}
	if (changed)
		updateVertexStream();
}

void
ParticleSystem::add(ModifierPtr	modifier)
{
	addComponents(modifier->getNeededComponents());
	
	IInitializerPtr i = std::dynamic_pointer_cast<modifier::IParticleInitializer> (modifier);

	if (i != 0)
	{
		_initializers.push_back(i);
		return;	
	}

	IUpdaterPtr u = std::dynamic_pointer_cast<modifier::IParticleUpdater> (modifier);

	if (u != 0)
		_updaters.push_back(u);
}

void
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
				updateVertexFormat();

				return;
			}
		}

		return;
	}
	
	IUpdaterPtr u = std::dynamic_pointer_cast<modifier::IParticleUpdater> (modifier);

	if (u != 0)
	{
		for (auto it = _updaters.begin(); it != _updaters.end(); ++it)
		{
			if (*it == u)
			{
				_updaters.erase(it);
				updateVertexFormat();

				return;
			}
		}
	}
}

bool
ParticleSystem::has(ModifierPtr 	modifier)
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
		float x = particle.x;
		float y = particle.y;
		float z = particle.z;

		particle.x = _localToWorld[0] * x + _localToWorld[4] * y + _localToWorld[8] * z + _localToWorld[12];
		particle.y = _localToWorld[1] * x + _localToWorld[5] * y + _localToWorld[9] * z + _localToWorld[13];
		particle.z = _localToWorld[2] * x + _localToWorld[6] * y + _localToWorld[10] * z + _localToWorld[14];


		if (_startDirection != StartDirection::NONE)
		{
			float vx = particle.startvx;
			float vy = particle.startvy;
			float vz = particle.startvz;

			particle.startvx = _localToWorld[0] * vx + _localToWorld[4] * vy + _localToWorld[8] * vz;
			particle.startvy = _localToWorld[1] * vx + _localToWorld[5] * vy + _localToWorld[9] * vz;
			particle.startvz = _localToWorld[2] * vx + _localToWorld[6] * vy + _localToWorld[10] * vz;
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
	unsigned int value = ceilf(_lifetime->max() / _rate - EPSILON);
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
	_liveCount = 0;

	for (unsigned particleIndex = 0; particleIndex < _particles.size(); ++particleIndex)
	{
		_particles[particleIndex].alive = false;
	}
}


void
ParticleSystem::addComponents(unsigned int components, bool blockVSInit)
{
	if (_format & components)
		return;

	_format |= components;

	VertexStreamPtr vs = _geometry->vertices();
	unsigned int vertexSize = 5;

	if (components & VertexComponentFlags::SIZE)
	{
		vs->addAttribute ("size", 1, vertexSize);
		vertexSize += 1;
	}

	if (components & VertexComponentFlags::COLOR)
	{
		vs->addAttribute ("color", 3, vertexSize);
		vertexSize += 3;
	}

	if (components & VertexComponentFlags::TIME)
	{
		vs->addAttribute ("time", 1, vertexSize);
		vertexSize += 1;
	}

	if (components & VertexComponentFlags::OLD_POSITION)
	{
		vs->addAttribute ("old_Position", 3, vertexSize);
		vertexSize += 3;
	}

	if (components & VertexComponentFlags::ROTATION)
	{
		vs->addAttribute ("rotation", 1, vertexSize);
		vertexSize += 1;
	}

	if (components & VertexComponentFlags::SPRITEINDEX)
	{
		vs->addAttribute ("spriteIndex", 1, vertexSize);
		vertexSize += 1;
	}

	if (!blockVSInit)
		_geometry->initStreams(_maxCount);
}

unsigned int
ParticleSystem::updateVertexFormat()
{
	_format = VertexComponentFlags::DEFAULT;
	
	_geometry->vertices()->addAttribute("offset", 2, 0);
	_geometry->vertices()->addAttribute("position", 3, 2);

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
ParticleSystem::updateVertexStream()
{
	if (_liveCount == 0)
		return;

	if (_isZSorted)
	{
		updateParticleDistancesToCamera();
		std::sort(_particleOrder.begin(), _particleOrder.end(), _comparisonObject);
	}
	
	std::vector<float>& vsData = _geometry->vertices()->data();
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
			setInVertexStream(vertexIterator, 2, particle->x);
			setInVertexStream(vertexIterator, 3, particle->y);
			setInVertexStream(vertexIterator, 4, particle->z);

			if (_format & VertexComponentFlags::SIZE)
				setInVertexStream(vertexIterator, i++, particle->size);

			if (_format & VertexComponentFlags::COLOR)
			{
				setInVertexStream(vertexIterator, i++, particle->r);
				setInVertexStream(vertexIterator, i++, particle->g);
				setInVertexStream(vertexIterator, i++, particle->b);
			}

			if (_format & VertexComponentFlags::TIME)
				setInVertexStream(vertexIterator, i++, particle->timeLived / particle->lifetime);

			if (_format & VertexComponentFlags::OLD_POSITION)
			{
				setInVertexStream(vertexIterator, i++, particle->oldx);
				setInVertexStream(vertexIterator, i++, particle->oldy);
				setInVertexStream(vertexIterator, i++, particle->oldz);
			}

			if (_format & VertexComponentFlags::ROTATION)
				setInVertexStream(vertexIterator, i++, particle->rotation);

			if (_format & VertexComponentFlags::SPRITEINDEX)
				setInVertexStream(vertexIterator, i++, particle->spriteIndex);

			vertexIterator += 4 * _geometry->vertexSize();
		}
	}
	_geometry->vertices()->upload();

	if (_liveCount != _previousLiveCount)
	{
		std::static_pointer_cast<render::ParticleIndexStream>(_geometry->indices())->upload(_liveCount);
		_previousLiveCount = _liveCount;
	}
}