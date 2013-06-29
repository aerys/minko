#pragma once

#include "minko/Common.hpp"
#include "minko/Any.hpp"
#include "minko/render/context/AbstractContext.hpp"
#include "minko/render/context/OpenGLES2Context.hpp"
#include "minko/render/GLSLProgram.hpp"
#include "minko/render/ShaderProgramInputs.hpp"
#include "minko/render/Effect.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/scene/data/DataProvider.hpp"
#include "minko/scene/data/Value.hpp"
#include "minko/scene/data/DataBindings.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/scene/controller/TransformController.hpp"
#include "minko/scene/controller/SurfaceController.hpp"
#include "minko/scene/controller/RenderingController.hpp"
#include "minko/scene/controller/PerspectiveCameraController.hpp"
#include "minko/render/stream/VertexAttribute.hpp"
#include "minko/render/stream/VertexFormat.hpp"
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/IndexStream.hpp"
#include "minko/scene/data/geometry/Geometry.hpp"
#include "minko/scene/data/geometry/CubeGeometry.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/EffectParser.hpp"

class Minko
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};
