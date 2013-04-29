#include "RenderingController.hpp"

void
RenderingController::initialize()
{
	targetAdded()->add(std::bind(
		&RenderingController::targetAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));	

	targetRemoved()->add(std::bind(
		&RenderingController::targetRemovedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
RenderingController::targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	_addedCd = target->added()->add(std::bind(
		&RenderingController::addedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));

	_removedCd = target->removed()->add(std::bind(
		&RenderingController::removedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
RenderingController::targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target)
{
	target->added()->remove(_addedCd);
	target->removed()->remove(_removedCd);
}

void
RenderingController::addedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	_rootDescendantAddedCd = node->root()->descendantAdded()->add(std::bind(
		&RenderingController::rootDescendantAddedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));

	_rootDescendantRemovedCd = node->root()->descendantRemoved()->add(std::bind(
		&RenderingController::rootDescendantRemovedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
	));
}

void
RenderingController::removedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	ancestor->root()->descendantAdded()->remove(_rootDescendantAddedCd);
	ancestor->root()->descendantRemoved()->remove(_rootDescendantRemovedCd);
}

void
RenderingController::rootDescendantAddedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
{
	// TODO: create draw calls
}

void
RenderingController::rootDescendantRemovedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor)
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
