#include "SurfaceController.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/data/geometry/Geometry.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/GLSLProgram.hpp"

using namespace minko::scene::controller;

SurfaceController::SurfaceController(std::shared_ptr<Geometry> 		geometry,
									 std::shared_ptr<DataProvider> 	material,
									 std::shared_ptr<Effect>		effect) :
	AbstractController(),
	_geometry(geometry),
	_material(material),
	_effect(effect),
	_drawCalls()
{
}

void
SurfaceController::initialize()
{
	_targetAddedSlot = targetAdded()->connect(std::bind(
		&SurfaceController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_targetRemovedSlot = targetRemoved()->connect(std::bind(
		&SurfaceController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
SurfaceController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->addProvider(_material);
	target->bindings()->addProvider(_geometry->data());
	target->bindings()->addProvider(_effect->data());

	_drawCalls.clear();
	for (auto shader : _effect->shaders())
	{
		auto drawCall = DrawCall::create(target->bindings(), _effect->inputNameToBindingName());

		_drawCalls.push_back(drawCall);
	}
}

void
SurfaceController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->removeProvider(_material);
	target->bindings()->removeProvider(_geometry->data());
	target->bindings()->removeProvider(_effect->data());

	_drawCalls.clear();
}
