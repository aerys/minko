#pragma once

#include "minko/Common.hpp"
#include "minko/scene/controller/AbstractController.hpp"

class RenderingController :
	public AbstractController,
	public std::enable_shared_from_this<RenderingController>
{
public:
	typedef std::shared_ptr<RenderingController>	ptr;

private:
	std::shared_ptr<OpenGLESContext>							_context;
	std::list<std::shared_ptr<DrawCall>>						_drawCalls;

	Signal<ptr>::ptr											_enterFrame;
	Signal<ptr>::ptr											_exitFrame;

	Signal<std::shared_ptr<Node>, std::shared_ptr<Node>>::cd	_addedCd;
	Signal<std::shared_ptr<Node>, std::shared_ptr<Node>>::cd	_removedCd;
	Signal<std::shared_ptr<Node>, std::shared_ptr<Node>>::cd	_rootAddedCd;
	Signal<std::shared_ptr<Node>, std::shared_ptr<Node>>::cd	_rootDescendantAddedCd;
	Signal<std::shared_ptr<Node>, std::shared_ptr<Node>>::cd	_rootDescendantRemovedCd;

public:
	static
	ptr
	create(std::shared_ptr<OpenGLESContext> context)
	{
		return std::shared_ptr<RenderingController>(new RenderingController(context));
	}

	void
	render();

	inline
	Signal<ptr>::ptr
	enterFrame()
	{
		return _enterFrame;
	}

	inline
	Signal<ptr>::ptr
	exitFrame()
	{
		return _exitFrame;
	}

private:
	RenderingController(std::shared_ptr<OpenGLESContext> context) :
		AbstractController(), _context(context)
	{
	}

	void
	initialize();

	void
	targetAddedHandler(std::shared_ptr<AbstractController>, std::shared_ptr<Node> target);

	void
	targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target);

	void
	addedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor);

	void
	removedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor);

	void
	rootDescendantAddedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor);

	void
	rootDescendantRemovedHandler(std::shared_ptr<Node> node, std::shared_ptr<Node> ancestor);

	void
	geometryChanged(std::shared_ptr<SurfaceController>);

	void
	materialChanged(std::shared_ptr<SurfaceController>);
};