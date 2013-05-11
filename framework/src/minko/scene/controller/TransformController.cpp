#include "TransformController.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko::scene::controller;
using namespace minko::scene::data;

TransformController::TransformController() :
	minko::scene::controller::AbstractController(),
	_transform(Matrix4x4::create()),
	_modelToWorld(Matrix4x4::create()),
	_data(DataProvider::create())
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
	_data->setProperty("transforms/worldToModelMatrix", _worldToModel);
}

void
TransformController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (targets().size() > 1)
		throw std::logic_error("TransformController cannot have more than one target.");
	if (target->controller<TransformController>(1) != nullptr)
		throw std::logic_error("A node cannot have more than one TransformController.");

	target->bindings()->addProvider(_data);
	
	_addedCd = target->added()->add(std::bind(
		&TransformController::addedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
	_removedCd = target->removed()->add(std::bind(
		&TransformController::addedOrRemovedHandler,
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

	target->added()->remove(_addedCd);
	target->removed()->remove(_removedCd);

	_referenceFrame = nullptr;
}

void
TransformController::addedOrRemovedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	updateReferenceFrame(node);
}

void
TransformController::controllerAddedOrRemovedHandler(std::shared_ptr<Node> 					node,
													 std::shared_ptr<AbstractController> 	ctrl)
{
	if (std::dynamic_pointer_cast<TransformController>(ctrl) != nullptr
		|| std::dynamic_pointer_cast<RenderingController>(ctrl) != nullptr)
		updateReferenceFrame(node);
}

void
TransformController::updateReferenceFrame(std::shared_ptr<Node> node)
{
	auto searchNode 		= node;
	auto newReferenceFrame 	= node;

	while (searchNode != nullptr)
	{
		if (searchNode->controller<TransformController>() != nullptr)
			newReferenceFrame = searchNode;

		searchNode = searchNode->parent();
	}

	if (newReferenceFrame == node && newReferenceFrame->controller<RootTransformController>() == nullptr)
		newReferenceFrame->addController(RootTransformController::create());
}

void
TransformController::TransformController::RootTransformController::initialize()
{
	targetAdded()->add(std::bind(
		&TransformController::RootTransformController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
TransformController::RootTransformController::targetAddedHandler(std::shared_ptr<AbstractController> 	ctrl,
																 std::shared_ptr<Node>					target)
{
	for (auto descendant : NodeSet::create(targets())->descendants()->nodes())
	{
		auto rootTransformCtrl = descendant->controller<RootTransformController>();

		if (rootTransformCtrl != nullptr)
			descendant->removeController(rootTransformCtrl);
	}

	auto callback = std::bind(
		&TransformController::RootTransformController::descendantAddedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	);

	_descendantAddedCd = target->descendantAdded()->add(callback);
	_descendantRemovedCd = target->descendantRemoved()->add(callback);

	updateEnterFrameListeners();
	updateControllerAddedOrRemovedListeners();
}

void
TransformController::RootTransformController::controllerAddedOrRemovedHandler(std::shared_ptr<Node>					node,
														 					  std::shared_ptr<AbstractController>	ctrl)
{
	if (std::dynamic_pointer_cast<RenderingController>(ctrl) != nullptr)
		updateEnterFrameListeners();
}

void
TransformController::RootTransformController::descendantAddedOrRemovedHandler(std::shared_ptr<Node> node,
														 					  std::shared_ptr<Node> descendant)
{
	updateEnterFrameListeners();
	updateControllerAddedOrRemovedListeners();
}

void
TransformController::RootTransformController::updateEnterFrameListeners()
{
	_enterFrameCds.clear();

	auto callback = std::bind(
		&TransformController::RootTransformController::enterFrameHandler,
		shared_from_this(),
		std::placeholders::_1
	);

	for (auto descendant : NodeSet::create(targets()[0]->root())->descendants(true)->nodes())
		for (auto renderingCtrl : descendant->controllers<RenderingController>())
			_enterFrameCds.push_back(renderingCtrl->enterFrame()->add(callback));
}

void
TransformController::RootTransformController::updateControllerAddedOrRemovedListeners()
{
	_controllerAddedOrRemovedCds.clear();

	auto callback = std::bind(
		&TransformController::RootTransformController::controllerAddedOrRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	);

	for (auto descendant : NodeSet::create(targets())->descendants(true)->nodes())
	{
		descendant->controllerAdded()->add(callback);
		descendant->controllerRemoved()->add(callback);
	}
}

void
TransformController::RootTransformController::enterFrameHandler(std::shared_ptr<RenderingController> ctrl)
{
	// TODO: update transforms list
}
