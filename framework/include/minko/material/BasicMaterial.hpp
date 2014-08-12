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

#include "minko/material/Material.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/render/TriangleCulling.hpp"

namespace minko
{
	namespace material
	{
		class BasicMaterial :
			public Material
		{
		public:
			typedef std::shared_ptr<BasicMaterial>				Ptr;

		protected:
			typedef std::shared_ptr<render::AbstractTexture>	AbsTexturePtr;
			typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::CubeTexture>		CubeTexturePtr;
			typedef std::shared_ptr<render::States>				RenderStatesPtr;

		protected:
			static const std::shared_ptr<render::States>		_defaultStates;

		public:
			inline static
			Ptr
			create()
			{
                return std::shared_ptr<BasicMaterial>(new BasicMaterial());
			}

			BasicMaterial&
			diffuseColor(const math::vec4&);

			BasicMaterial&
			diffuseColor(uint);

			math::vec4
			diffuseColor() const;

			BasicMaterial&
			uvScale(const math::vec2& v);

			const math::vec2&
			uvScale() const;

			BasicMaterial&
			uvOffset(const math::vec2& v);

			const math::vec2&
			uvOffset() const;

			BasicMaterial&
            diffuseMap(TexturePtr);

			render::ResourceId
			diffuseMap() const;

			BasicMaterial&
			diffuseCubeMap(AbsTexturePtr);

			render::ResourceId
			diffuseCubeMap() const;

			BasicMaterial&
			fogColor(const math::vec4&);

			BasicMaterial&
			fogColor(uint);

			math::vec4
			fogColor() const;

            BasicMaterial&
            fogDensity(float);

            float
            fogDensity() const;

            BasicMaterial&
            fogStart(float);

            float
            fogStart() const;

            BasicMaterial&
            fogEnd(float);

            float
            fogEnd() const;

            BasicMaterial&
            fogType(render::FogType);

            render::FogType
            fogType() const;

			BasicMaterial&
			blendingMode(render::Blending::Source, render::Blending::Destination);

			BasicMaterial&
			blendingMode(render::Blending::Mode);

			render::Blending::Source
			blendingSourceFactor() const;

			render::Blending::Destination
			blendingDestinationFactor() const;

			BasicMaterial&
			colorMask(bool);

			bool
			colorMask() const;

			BasicMaterial&
			depthMask(bool);

			bool
			depthMask() const;

			BasicMaterial&
			depthFunction(render::CompareMode);

			render::CompareMode
			depthFunction() const;

			BasicMaterial&
			triangleCulling(render::TriangleCulling);

			render::TriangleCulling
			triangleCulling() const;

			BasicMaterial&
			stencilFunction(render::CompareMode);

			render::CompareMode
			stencilFunction() const;

			BasicMaterial&
			stencilReference(int);

			int
			stencilReference() const;

			BasicMaterial&
			stencilMask(uint);

			uint
			stencilMask() const;

			BasicMaterial&
			stencilFailOperation(render::StencilOperation);

			render::StencilOperation
			stencilFailOperation() const;

			BasicMaterial&
			stencilDepthFailOperation(render::StencilOperation);

			render::StencilOperation
			stencilDepthFailOperation() const;

			BasicMaterial&
			stencilDepthPassOperation(render::StencilOperation);

			render::StencilOperation
			stencilDepthPassOperation() const;

			BasicMaterial&
			priority(float);

			float 
			priority() const;

			BasicMaterial&
			zSorted(bool);

			bool
			zSorted() const;

		protected:
			BasicMaterial();
		};
	}
}
