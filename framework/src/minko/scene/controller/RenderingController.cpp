#include "RenderingController.hpp"
#include "minko/scene/Node.hpp"

using namespace minko::scene::controller;

void
RenderingController::initialize()
{
	targetAdded()->add(std::bind(
		&RenderingController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	targetRemoved()->add(std::bind(
		&RenderingController::targetRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));
}

void
RenderingController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl,
										std::shared_ptr<Node> 				target)
{
	if (target->controllers<RenderingController>().size() > 1)
		throw std::logic_error("There cannot be two RenderingController on the same node.");

	_addedCd = target->added()->add(std::bind(
		&RenderingController::addedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedCd = target->removed()->add(std::bind(
		&RenderingController::removedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
RenderingController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	_addedCd = nullptr;
	_removedCd = nullptr;
}

void
RenderingController::addedHandler(std::shared_ptr<Node> node,
								  std::shared_ptr<Node> target,
								  std::shared_ptr<Node> parent)
{
	_rootDescendantAddedCd = node->root()->added()->add(std::bind(
		&RenderingController::rootDescendantAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedCd = node->root()->removed()->add(std::bind(
		&RenderingController::rootDescendantRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));
}

void
RenderingController::removedHandler(std::shared_ptr<Node> node,
									std::shared_ptr<Node> target,
									std::shared_ptr<Node> parent)
{
	_rootDescendantAddedCd = nullptr;
	_rootDescendantRemovedCd = nullptr;
}

void
RenderingController::rootDescendantAddedHandler(std::shared_ptr<Node> node,
												std::shared_ptr<Node> target,
												std::shared_ptr<Node> parent)
{
	// TODO: create draw calls
}

void
RenderingController::rootDescendantRemovedHandler(std::shared_ptr<Node> node,
												  std::shared_ptr<Node> target,
												  std::shared_ptr<Node> parent)
{
	// TODO: delete draw calls
}

void
RenderingController::geometryChanged(std::shared_ptr<SurfaceController>)
{
	// TODO: update draw call
}

void
RenderingController::materialChanged(std::shared_ptr<SurfaceController>)
{
	// TODO: update draw call
}

void
RenderingController::render()
{
	_enterFrame->execute(shared_from_this());

	// TODO: do draw calls...

	_exitFrame->execute(shared_from_this());
}
