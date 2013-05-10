#include "TransformController.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/scene/Scene.hpp"
#include "minko/scene/Node.hpp"

using namespace minko::scene::controller;
using namespace minko::scene::data;

TransformController::TransformController() :
	minko::scene::controller::AbstractController(),
	_transform(Matrix4x4::create()),
	_modelToWorld(Matrix4x4::create()),
	_data(DataProvider::create()),
	_referenceFrame(nullptr)
{
}

void
TransformController::initialize()
{
	targetAdded()->add(std::bind(
		&TransformController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	targetRemoved()->add(std::bind(
		&TransformController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	_data->setProperty("transforms/modelToWorldMatrix", _modelToWorld);
}

void
TransformController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (targets().size() > 1)
		throw std::logic_error("TransformController cannot have more than one target.");

	target->bindings()->addProvider(_data);
	target->added()->add(std::bind(
		&TransformController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
	target->removed()->add(std::bind(
		&TransformController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));

	updateReferenceFrame(target);
}

void
TransformController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	target->bindings()->removeProvider(_data);
}

void
TransformController::addedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	updateReferenceFrame(node);
}

void
TransformController::removedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	updateReferenceFrame(node);
}

void
TransformController::enterFrameHandler(std::shared_ptr<Scene> scene)
{
	std::cout << "TransformController::enterFrameHandler()" << std::endl;
}

void
TransformController::updateReferenceFrame(std::shared_ptr<Node> node)
{
	std::shared_ptr<Node> searchNode = node;

	while (searchNode != nullptr)
	{
		if (searchNode->controller<TransformController>() != nullptr)
			_referenceFrame = searchNode;

		searchNode = searchNode->parent();
	}

	if (_referenceFrame == node && node->scene() != nullptr)
	{
		node->scene()->enterFrame()->add(std::bind(
			&TransformController::enterFrameHandler,
			shared_from_this(),
			std::placeholders::_1
		));
	}
}
