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

#include "minko/serialize/ComponentSerializer.hpp"
#include "msgpack.hpp"
#include "minko/Types.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"
#include "minko/component/PointLight.hpp"
#include "minko/component/SpotLight.hpp"
#include "minko/component/Surface.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/Effect.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/math/Vector3.hpp"
#include "minko/math/Box.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/file/Dependency.hpp"

using namespace minko;
using namespace minko::serialize;


std::string
ComponentSerializer::serializeTransform(NodePtr			node, 
										DependencyPtr	dependencies)
{
	int8_t										type		= serialize::TRANSFORM;
	std::shared_ptr<component::Transform>		transform	= node->component<component::Transform>();
	std::stringstream							buffer;
	std::tuple<uint, std::string>				serialized	= serialize::TypeSerializer::serializeMatrix4x4(transform->matrix());
	msgpack::type::tuple<uint, std::string>		src(std::get<0>(serialized), std::get<1>(serialized));

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializePerspectiveCamera(NodePtr			node, 
												DependencyPtr	dependencies)
{
	int8_t											type = serialize::PROJECTION_CAMERA;
	std::shared_ptr<component::PerspectiveCamera>	perspectiveCamera = node->component<component::PerspectiveCamera>();
	std::stringstream								buffer;
	std::vector<float>								cameraDatas(4, 0);

	cameraDatas[0] = perspectiveCamera->aspectRatio();
	cameraDatas[1] = perspectiveCamera->fieldOfView();
	cameraDatas[2] = perspectiveCamera->zNear();
	cameraDatas[3] = perspectiveCamera->zFar();

	msgpack::type::tuple<std::vector<float>>	src(cameraDatas);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}


std::string
ComponentSerializer::serializeAmbientLight(NodePtr			node, 
										   DependencyPtr	dependencies)
{
	int8_t												type		= serialize::AMBIENT_LIGHT;
	std::shared_ptr<component::AmbientLight>			ambient		= node->component<component::AmbientLight>();
	std::stringstream									buffer;
	msgpack::type::tuple<float, float, float, float>	src(
		ambient->ambient(),
		ambient->color()->x(),
		ambient->color()->y(),
		ambient->color()->z());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}
			
std::string
ComponentSerializer::serializeDirectionalLight(NodePtr			node, 
											   DependencyPtr	dependencies)
{
	int8_t													type		= serialize::DIRECTIONAL_LIGHT;
	std::shared_ptr<component::DirectionalLight>			directional	= node->component<component::DirectionalLight>();
	std::stringstream										buffer;
	msgpack::type::tuple<float, float, float, float, float>	src(
		directional->diffuse(),
		directional->specular(),
		directional->color()->x(),
		directional->color()->y(),
		directional->color()->z());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}


std::string
ComponentSerializer::serializePointLight(NodePtr		node, 
										 DependencyPtr	dependencies)
{
	int8_t									type	= serialize::POINT_LIGHT;
	std::shared_ptr<component::PointLight>	point	= node->component<component::PointLight>();
	std::stringstream						buffer;
	msgpack::type::tuple<float, float, float, float, float, float>	src(
		point->diffuse(),
		point->specular(),
		point->attenuationDistance(),
		point->color()->x(),
		point->color()->y(),
		point->color()->z());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeSpotLight(NodePtr			node, 
										DependencyPtr	dependencies)
{
	int8_t										type	= serialize::SPOT_LIGHT;
	std::shared_ptr<component::SpotLight>		spot	= node->component<component::SpotLight>();
	std::stringstream							buffer;
	msgpack::type::tuple<float, float, float, float, float, float, float, float> src(
		spot->diffuse(),
		spot->specular(),
		spot->attenuationDistance(),
		spot->cosInnerConeAngle(),
		spot->cosOuterConeAngle(),
		spot->color()->x(),
		spot->color()->y(),
		spot->color()->z());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeSurface(NodePtr		node, 
									  DependencyPtr dependencies)
{
	int8_t									type	= serialize::SURFACE;
	std::shared_ptr<component::Surface>		surface	= node->component<component::Surface>();
	std::stringstream						buffer;

	uint materialId = dependencies->registerDependency(surface->material());
	uint geometryId = dependencies->registerDependency(surface->geometry());
	uint effectId	= dependencies->registerDependency(surface->effect());

	msgpack::type::tuple<unsigned short, unsigned short, unsigned short, std::string> src(
		geometryId,
		materialId,
		effectId,
		surface->technique());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}

std::string
ComponentSerializer::serializeRenderer(NodePtr			node, 
									   DependencyPtr	dependencies)
{
	int8_t									type		= serialize::RENDERER;
	std::shared_ptr<component::Renderer>	renderer	= node->component<component::Renderer>();
	std::stringstream						buffer;

	msgpack::type::tuple<unsigned int> src(renderer->backgroundColor());

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}


std::string
ComponentSerializer::serializeBoundingBox(NodePtr 			node,
					 				      DependencyPtr 	dependencies)
{
	math::Box::Ptr 		box 		= node->component<component::BoundingBox>()->box();
	math::Vector3::Ptr 	topRight 	= box->topRight();
	math::Vector3::Ptr 	bottomLeft 	= box->bottomLeft();
	int8_t 				type 		= serialize::BOUNDINGBOX;
	std::stringstream	buffer;

	float centerX = topRight->x() - bottomLeft->x();
	float centerY = topRight->y() - bottomLeft->y();
	float centerZ = topRight->z() - bottomLeft->z();

	msgpack::type::tuple<float, float, float, float, float, float> src(
		centerX,
		centerY,
		centerZ,
		box->width(),
		box->height(),
		box->depth()
		);

	msgpack::pack(buffer, src);
	msgpack::pack(buffer, type);

	return buffer.str();
}