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

#include "minko/material/WaterMaterial.hpp"
#include "minko/Color.hpp"
#include "minko/math/Vector4.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

using namespace minko;
using namespace minko::material;

WaterMaterial::WaterMaterial(uint numWaves):
_numWaves(numWaves)
{

}

void
WaterMaterial::initialize()
{
	BasicMaterial::initialize();

	_amplitudes.resize(_numWaves, 0.001f);
	_origins.resize(_numWaves * 2, 0.f);
	_waveLength.resize(_numWaves, 0.1f);
	_sharpness.resize(_numWaves, 0.f);
	_speeds.resize(_numWaves, 1.f);
	_waveType.resize(_numWaves, 0.f);
	

	set<int>("numWaves", _numWaves);
	specularColor(0xffffffff);
	shininess(8.0f);
	reflectivity(0.0f);
	normalMapSpeed(0.005f);
	normalMapScale(0.01f);
	flowMapScale(1.f);
	flowMapCycle(1.f);
	flowMapOffset1(0.f);
	flowMapOffset2(0.f);
	dudvFactor(1.f);
	dudvSpeed(0.01f);
	fresnelPow(1.f);
	fresnelMultiplier(1.0f);
	fogColor(0x909090FF);
	fogDensity(0.001f);
	fogStart(100.f);
	fogEnd(400.0f);
	fogType(render::FogType::Exponential);

	data::UniformArrayPtr<float>	amplitudesUniformArray(new data::UniformArray<float>(_numWaves, &(_amplitudes[0])));
	data::UniformArrayPtr<float>	originsUniformArray(new data::UniformArray<float>(_numWaves * 2, &(_origins[0])));
	data::UniformArrayPtr<float>	waveLengthUniformArray(new data::UniformArray<float>(_numWaves, &(_waveLength[0])));
	data::UniformArrayPtr<float>	sharpnessUniformArray(new data::UniformArray<float>(_numWaves, &(_sharpness[0])));
	data::UniformArrayPtr<float>	speedsUniformArray(new data::UniformArray<float>(_numWaves, &(_speeds[0])));
	data::UniformArrayPtr<float>	waveTypeUniformArray(new data::UniformArray<float>(_numWaves, &(_waveType[0])));

	set<data::UniformArrayPtr<float>>("waveOrigins",	originsUniformArray);
	set<data::UniformArrayPtr<float>>("waveLength",		waveLengthUniformArray);
	set<data::UniformArrayPtr<float>>("waveAmplitudes", amplitudesUniformArray);
	set<data::UniformArrayPtr<float>>("waveSharpness",	sharpnessUniformArray);
	set<data::UniformArrayPtr<float>>("waveSpeed",		speedsUniformArray);
	set<data::UniformArrayPtr<float>>("waveType",		waveTypeUniformArray);
}


WaterMaterial::Ptr
WaterMaterial::setDirection(int waveId, std::shared_ptr<math::Vector2> direction)
{
	_origins[waveId * 2] = direction->x();
	_origins[waveId * 2 + 1] = direction->y();
	_waveType[waveId] = 0;
	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setCenter(int waveId, std::shared_ptr<math::Vector2> origin)
{
	_origins[waveId * 2] = origin->x();
	_origins[waveId * 2 + 1] = origin->y();
	_waveType[waveId] = 1;
	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setAmplitude(int waveId, float amplitude)
{
	_amplitudes[waveId] = amplitude;
	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setWaveLenght(int waveId, float waveLenght)
{
	_waveLength[waveId] = waveLenght;

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setSharpness(int waveId, float sharpness)
{
	_sharpness[waveId] = sharpness;

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setSpeed(int waveId, float speed)
{
	_speeds[waveId] = speed;

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::reflectionMap(render::AbstractTexture::Ptr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d reflection maps are currently supported.");

	set("reflectionMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

render::Texture::Ptr
WaterMaterial::reflectionMap() const
{
	return hasProperty("reflectionMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("reflectionMap"))
		: nullptr;
}

WaterMaterial::Ptr
WaterMaterial::depthMap(render::AbstractTexture::Ptr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d depth maps are currently supported.");

	set("depthMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

render::Texture::Ptr
WaterMaterial::depthMap() const
{
	return hasProperty("depthMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("depthMap"))
		: nullptr;
}

WaterMaterial::Ptr
WaterMaterial::dudvMap(render::AbstractTexture::Ptr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d dudv maps are currently supported.");

	set("dudvMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

render::Texture::Ptr
WaterMaterial::dudvMap() const
{
	return hasProperty("dudvMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("dudvMap"))
		: nullptr;
}

WaterMaterial::Ptr
WaterMaterial::dudvSpeed(float s)
{
	set("dudvSpeed", s);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::dudvSpeed() const
{
	return get<float>("dudvSpeed");
}

WaterMaterial::Ptr
WaterMaterial::dudvFactor(float s)
{
	set("dudvFactor", s);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::dudvFactor() const
{
	return get<float>("dudvFactor");
}

WaterMaterial::Ptr
WaterMaterial::fresnelMultiplier(float s)
{
	set("fresnelMultiplier", s);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::fresnelMultiplier() const
{
	return get<float>("fresnelMultiplier");
}


WaterMaterial::Ptr
WaterMaterial::normalMultiplier(float value)
{
	set("normalMultiplier", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::normalMultiplier() const
{
	return get<float>("normalMultiplier");
}

WaterMaterial::Ptr
WaterMaterial::fresnelPow(float value)
{
	set("fresnelPow", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::fresnelPow() const
{
	return get<float>("fresnelPow");
}

WaterMaterial::Ptr
WaterMaterial::normalMap(render::AbstractTexture::Ptr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d normal maps are currently supported.");

	set("normalMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

render::Texture::Ptr
WaterMaterial::normalMap() const
{
	return hasProperty("normalMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("normalMap"))
		: nullptr;
}


WaterMaterial::Ptr
WaterMaterial::flowMap(AbsTexturePtr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d flow maps are currently supported.");

	set("flowMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

render::Texture::Ptr
WaterMaterial::flowMap() const
{
	return hasProperty("flowMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("flowMap"))
		: nullptr;
}

WaterMaterial::Ptr
WaterMaterial::noiseMap(AbsTexturePtr value)
{
	if (value->type() == render::TextureType::CubeTexture)
		throw new std::logic_error("Only 2d noise maps are currently supported.");

	set("noiseMap", std::static_pointer_cast<render::AbstractTexture>(value));

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::TexturePtr
WaterMaterial::noiseMap() const
{
	return hasProperty("noiseMap")
		? std::dynamic_pointer_cast<render::Texture>(get<render::AbstractTexture::Ptr>("noiseMap"))
		: nullptr;
}

WaterMaterial::Ptr
WaterMaterial::normalMapSpeed(float s)
{
	set("normalSpeed", s);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::normalMapSpeed() const
{
	return get<float>("normalSpeed");
}

WaterMaterial::Ptr
WaterMaterial::specularColor(math::Vector4::Ptr color)
{
	set("specularColor", color);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::specularColor(uint color)
{
	return specularColor(Color::uintToVec4(color));
}

math::Vector4::Ptr
WaterMaterial::specularColor() const
{
	return get<math::Vector4::Ptr>("specularColor");
}

WaterMaterial::Ptr
WaterMaterial::shininess(float value)
{
	set("shininess", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::shininess() const
{
	return get<float>("shininess");
}

WaterMaterial::Ptr
WaterMaterial::normalMapScale(float value)
{
	set("normalMapScale", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::normalMapScale() const
{
	return get<float>("normalMapScale");
}

WaterMaterial::Ptr
WaterMaterial::flowMapScale(float value)
{
	set("flowMapScale", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::flowMapScale() const
{
	return get<float>("flowMapScale");
}

WaterMaterial::Ptr
WaterMaterial::flowMapCycle(float value)
{
	set("flowMapCycle", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::flowMapCycle() const
{
	return get<float>("flowMapCycle");
}

WaterMaterial::Ptr
WaterMaterial::reflectivity(float value)
{
	set("reflectivity", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::reflectivity() const
{
	return get<float>("reflectivity");
}

WaterMaterial::Ptr
WaterMaterial::flowMapOffset1(float value)
{
	set("flowMapOffset1", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::flowMapOffset1() const
{
	return get<float>("flowMapOffset1");
}

WaterMaterial::Ptr
WaterMaterial::flowMapOffset2(float value)
{
	set("flowMapOffset2", value);

	return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

float
WaterMaterial::flowMapOffset2() const
{
	return get<float>("flowMapOffset2");
}