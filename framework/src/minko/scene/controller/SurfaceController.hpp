#pragma once

#include <functional>
#include <memory>

#include "minko/Minko.hpp"

class SurfaceController :
	public AbstractController,
	public std::enable_shared_from_this<SurfaceController>
{
public:
	typedef std::shared_ptr<SurfaceController>	ptr;

public:
	static
	ptr
	create(std::shared_ptr<Geometry> geometry, std::shared_ptr<DataProvider> material)
	{
		ptr surface(new SurfaceController(geometry, material));

		surface->initialize();

		return surface;
	}

private:
	std::shared_ptr<Geometry>		_geometry;
	std::shared_ptr<DataProvider>	_material;

	std::shared_ptr<DrawCall>		_drawCall;

private:
	SurfaceController(std::shared_ptr<Geometry> geometry, std::shared_ptr<DataProvider> material);

	void
	initialize();

	void
	targetAddedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target);

	void
	targetRemovedHandler(std::shared_ptr<AbstractController> ctrl, std::shared_ptr<Node> target);
};