#include "RenderingController.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/controller/SurfaceController.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/context/AbstractContext.hpp"

using namespace minko::scene::controller;

void
RenderingController::initialize()
{
	_targetAddedCd = targetAdded()->add(std::bind(
		&RenderingController::targetAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2
	));	

	_targetRemovedCd = targetRemoved()->add(std::bind(
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

	addedHandler(target->root(), target, target->parent());
}

void
RenderingController::targetRemovedHandler(std::shared_ptr<AbstractController> 	ctrl,
										  std::shared_ptr<Node> 				target)
{
	_addedCd = nullptr;
	_removedCd = nullptr;

	removedHandler(target->root(), target, target->parent());
}

void
RenderingController::addedHandler(std::shared_ptr<Node> node,
								  std::shared_ptr<Node> target,
								  std::shared_ptr<Node> parent)
{
	_rootDescendantAddedCd = target->root()->added()->add(std::bind(
		&RenderingController::rootDescendantAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_rootDescendantRemovedCd = target->root()->removed()->add(std::bind(
		&RenderingController::rootDescendantRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_controllerAddedCd = target->root()->controllerAdded()->add(std::bind(
		&RenderingController::controllerAddedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_controllerRemovedCd = target->root()->controllerRemoved()->add(std::bind(
		&RenderingController::controllerRemovedHandler,
		shared_from_this(),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	rootDescendantAddedHandler(target->root(), target, target->parent());
}

void
RenderingController::removedHandler(std::shared_ptr<Node> node,
									std::shared_ptr<Node> target,
									std::shared_ptr<Node> parent)
{
	_rootDescendantAddedCd = nullptr;
	_rootDescendantRemovedCd = nullptr;
	_controllerAddedCd = nullptr;
	_controllerRemovedCd = nullptr;

	rootDescendantRemovedHandler(target->root(), target, target->parent());
}

void
RenderingController::rootDescendantAddedHandler(std::shared_ptr<Node> node,
												std::shared_ptr<Node> target,
												std::shared_ptr<Node> parent)
{
	auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
		->where([](std::shared_ptr<Node>	 node)
		{
			return node->hasController<SurfaceController>();
		});

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->controllers<SurfaceController>())
			addSurfaceController(surface);
}

void
RenderingController::rootDescendantRemovedHandler(std::shared_ptr<Node> node,
												  std::shared_ptr<Node> target,
												  std::shared_ptr<Node> parent)
{
	auto surfaceNodes = NodeSet::create(target)
		->descendants(true)
		->where([](std::shared_ptr<Node>	 node)
		{
			return node->hasController<SurfaceController>();
		});

	for (auto surfaceNode : surfaceNodes->nodes())
		for (auto surface : surfaceNode->controllers<SurfaceController>())
			removeSurfaceController(surface);
}

void
RenderingController::controllerAddedHandler(std::shared_ptr<Node>				node,
											std::shared_ptr<Node>				target,
											std::shared_ptr<AbstractController>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<SurfaceController>(ctrl);
	
	if (surfaceCtrl)
		addSurfaceController(surfaceCtrl);
}

void
RenderingController::controllerRemovedHandler(std::shared_ptr<Node>					node,
											  std::shared_ptr<Node>					target,
											  std::shared_ptr<AbstractController>	ctrl)
{
	auto surfaceCtrl = std::dynamic_pointer_cast<SurfaceController>(ctrl);

	if (surfaceCtrl)
		removeSurfaceController(surfaceCtrl);
}

void
RenderingController::addSurfaceController(std::shared_ptr<SurfaceController> ctrl)
{
	_drawCalls.insert(_drawCalls.end(), ctrl->drawCalls().begin(), ctrl->drawCalls().end());
}

void
RenderingController::removeSurfaceController(std::shared_ptr<SurfaceController> ctrl)
{
#ifdef __GNUC__
  // Temporary non-fix for GCC missing feature N2350: http://gcc.gnu.org/onlinedocs/libstdc++/manual/status.html
#else
        _drawCalls.erase(ctrl->drawCalls().begin(), ctrl->drawCalls().end());
#endif
}

void
RenderingController::render()
{
	_enterFrame->execute(shared_from_this());

	_context->clear(.5f, .5f, .5f, 1.f);

	for (auto drawCall : _drawCalls)
		drawCall->render(_context);

	_context->present();

	_exitFrame->execute(shared_from_this());
}
