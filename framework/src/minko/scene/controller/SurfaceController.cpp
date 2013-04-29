#include "SurfaceController.hpp"

#include <iostream>

SurfaceController::SurfaceController(std::shared_ptr<Geometry> geometry, std::shared_ptr<DataProvider> material)
	: AbstractController(), _geometry(geometry), _material(material)
{
}

void
SurfaceController::initialize()
{
	targetAdded()->add(std::bind(
		&SurfaceController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
	targetRemoved()->add(std::bind(
		&SurfaceController::targetRemovedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
SurfaceController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->addProvider(_material);
}

void
SurfaceController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->bindings()->removeProvider(_material);
}