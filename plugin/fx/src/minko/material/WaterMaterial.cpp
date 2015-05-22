/*
Copyright (c) 2014 Aerys

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
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"

using namespace minko;
using namespace minko::material;

WaterMaterial::WaterMaterial(uint numWaves, const std::string& name) :
    PhongMaterial(name),
    _numWaves(numWaves),
    _amplitudes(numWaves, 0.f),
    _origins(numWaves * 2, math::vec2(1.f)),
    _waveLength(numWaves, 0.f),
    _speeds(numWaves, 0.f),
    _sharpness(numWaves, 0.f),
    _waveType(numWaves, 0)
{
    data()
        ->set("numWaves",       _numWaves)
        ->set("waveOrigin",     _origins)
        ->set("waveLength",     _waveLength)
        ->set("waveAmplitude",  _amplitudes)
        ->set("waveSharpness",  _sharpness)
        ->set("waveSpeed",      _speeds)
        ->set("waveType",       _waveType);
}

WaterMaterial::Ptr
WaterMaterial::setDirection(int waveId, const math::vec2& direction)
{
    setWaveProperty("waveOrigin", waveId, direction);
    setWaveProperty("waveType", waveId, 0);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setCenter(int waveId, const math::vec2& origin)
{
    setWaveProperty("waveOrigin", waveId, origin);
    setWaveProperty("waveType", waveId, 1);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setAmplitude(int waveId, float amplitude)
{
    setWaveProperty("waveAmplitude", waveId, amplitude);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setWaveLength(int waveId, float waveLength)
{
    setWaveProperty("waveLength", waveId, waveLength);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setSharpness(int waveId, float sharpness)
{
    setWaveProperty("waveSharpness", waveId, sharpness);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

WaterMaterial::Ptr
WaterMaterial::setSpeed(int waveId, float speed)
{
    setWaveProperty("waveSpeed", waveId, speed);

    return std::static_pointer_cast<WaterMaterial>(shared_from_this());
}

// WaterMaterial::Ptr
// WaterMaterial::reflectionMap(render::AbstractTexture::Ptr value)
// {
//     if (value->type() == render::TextureType::CubeTexture)
//         throw new std::logic_error("Only 2d reflection maps are currently supported.");
//
//     data()->set("reflectionMap", value->sampler());
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }

// WaterMaterial::Ptr
// WaterMaterial::depthMap(render::AbstractTexture::Ptr value)
// {
//     if (value->type() == render::TextureType::CubeTexture)
//         throw new std::logic_error("Only 2d depth maps are currently supported.");
//
//     data()->set("depthMap", value->sampler());
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }

// WaterMaterial::Ptr
// WaterMaterial::dudvMap(render::AbstractTexture::Ptr value)
// {
//     if (value->type() == render::TextureType::CubeTexture)
//         throw new std::logic_error("Only 2d dudv maps are currently supported.");
//
//     data()->set("dudvMap", value->sampler());
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// WaterMaterial::Ptr
// WaterMaterial::dudvSpeed(float s)
// {
//     data()->set("dudvSpeed", s);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::dudvSpeed() const
// {
//     return data()->get<float>("dudvSpeed");
// }
//
// WaterMaterial::Ptr
// WaterMaterial::dudvFactor(float s)
// {
//     data()->set("dudvFactor", s);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::dudvFactor() const
// {
//     return data()->get<float>("dudvFactor");
// }

// WaterMaterial::Ptr
// WaterMaterial::flowMap(AbsTexturePtr value)
// {
//     if (value->type() == render::TextureType::CubeTexture)
//         throw new std::logic_error("Only 2d flow maps are currently supported.");
//
//     data()->set("flowMap", value->sampler());
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// WaterMaterial::Ptr
// WaterMaterial::noiseMap(AbsTexturePtr value)
// {
//     if (value->type() == render::TextureType::CubeTexture)
//         throw new std::logic_error("Only 2d noise maps are currently supported.");
//
//     data()->set("noiseMap", value->sampler());
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }

// WaterMaterial::Ptr
// WaterMaterial::flowMapScale(float value)
// {
//     data()->set("flowMapScale", value);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::flowMapScale() const
// {
//     return data()->get<float>("flowMapScale");
// }
//
// WaterMaterial::Ptr
// WaterMaterial::flowMapCycle(float value)
// {
//     data()->set("flowMapCycle", value);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::flowMapCycle() const
// {
//     return data()->get<float>("flowMapCycle");
// }

// WaterMaterial::Ptr
// WaterMaterial::flowMapOffset1(float value)
// {
//     data()->set("flowMapOffset1", value);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::flowMapOffset1() const
// {
//     return data()->get<float>("flowMapOffset1");
// }
//
// WaterMaterial::Ptr
// WaterMaterial::flowMapOffset2(float value)
// {
//     data()->set("flowMapOffset2", value);
//
//     return std::static_pointer_cast<WaterMaterial>(shared_from_this());
// }
//
// float
// WaterMaterial::flowMapOffset2() const
// {
//     return data()->get<float>("flowMapOffset2");
// }
