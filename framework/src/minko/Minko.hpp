/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
#include "minko/render/stream/VertexStream.hpp"
#include "minko/render/stream/IndexStream.hpp"
#include "minko/scene/data/geometry/Geometry.hpp"
#include "minko/scene/data/geometry/CubeGeometry.hpp"
#include "minko/scene/data/geometry/SphereGeometry.hpp"
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
