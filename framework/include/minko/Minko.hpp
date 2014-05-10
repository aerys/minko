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
#include "minko/Color.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/render/ProgramInputs.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/Shader.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/ProgramSignature.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/math/Vector2.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/math/AbstractShape.hpp"
#include "minko/math/Box.hpp"
#include "minko/math/Ray.hpp"
#include "minko/math/Frustum.hpp"
#include "minko/Signal.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/ArrayProvider.hpp"
#include "minko/data/StructureProvider.hpp"
#include "minko/data/Value.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/ContainerProperty.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/OrthographicCamera.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/AbstractLight.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/AbstractDiscreteLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/MousePicking.hpp"
#include "minko/component/MouseManager.hpp"
#include "minko/component/SkinningMethod.hpp"
#include "minko/component/Culling.hpp"
#include "minko/component/Picking.hpp"
#include "minko/component/AbstractAnimation.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/Animation.hpp"
#include "minko/animation/AbstractTimeline.hpp"
#include "minko/animation/Matrix4x4Timeline.hpp"
#include "minko/component/JobManager.hpp"
#include "minko/render/AbstractResource.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/VertexBuffer.hpp"
#include "minko/render/IndexBuffer.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Priority.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/geometry/CubeGeometry.hpp"
#include "minko/geometry/SphereGeometry.hpp"
#include "minko/geometry/QuadGeometry.hpp"
#include "minko/geometry/TeapotGeometry.hpp"
#include "minko/geometry/LineGeometry.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AbstractLoader.hpp"
#include "minko/file/FileLoader.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/material/Material.hpp"
#include "minko/material/BasicMaterial.hpp"
#include "minko/material/PhongMaterial.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/input/Keyboard.hpp"

