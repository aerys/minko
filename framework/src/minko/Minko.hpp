#pragma once

#include "minko/Common.hpp"
#include "minko/Any.hpp"
#include "minko/render/context/AbstractContext.hpp"
#include "minko/render/context/OpenGLESContext.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/ShaderProgramInputs.hpp"
#include "minko/render/Effect.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/Scene.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/data/DataProvider.hpp"
#include "minko/scene/data/DataBindings.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/render/geometry/Geometry.hpp"
#include "minko/render/geometry/CubeGeometry.hpp"
#include "minko/scene/controller/TransformController.hpp"
#include "minko/scene/controller/SurfaceController.hpp"
#include "minko/scene/controller/RenderingController.hpp"

class Minko
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};
