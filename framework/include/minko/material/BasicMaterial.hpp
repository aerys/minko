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
			static const RenderStatesPtr						_defaultStates;

		public:
			inline static
			Ptr
			create()
			{
				Ptr ptr = std::shared_ptr<BasicMaterial>(new BasicMaterial());

				ptr->initialize();

				return ptr;
			}

			Ptr
			diffuseColor(const math::vec4&);

			Ptr
			diffuseColor(uint);

			math::vec4
			diffuseColor() const;

			Ptr
			diffuseMap(AbsTexturePtr);

			TexturePtr
			diffuseMap() const;

			Ptr
			diffuseCubeMap(AbsTexturePtr);

			CubeTexturePtr
			diffuseCubeMap() const;

			Ptr
			fogColor(const math::vec4&);

			Ptr
			fogColor(uint);

			math::vec4
			fogColor() const;

            Ptr
            fogDensity(float);

            float
            fogDensity() const;

            Ptr
            fogStart(float);

            float
            fogStart() const;

            Ptr
            fogEnd(float);

            float
            fogEnd() const;

            Ptr
            fogType(render::FogType);

            render::FogType
            fogType() const;

			Ptr
			blendingMode(render::Blending::Source, render::Blending::Destination);

			Ptr
			blendingMode(render::Blending::Mode);

			render::Blending::Source
			blendingSourceFactor() const;

			render::Blending::Destination
			blendingDestinationFactor() const;

			Ptr
			colorMask(bool);

			bool
			colorMask() const;

			Ptr
			depthMask(bool);

			bool
			depthMask() const;

			Ptr
			depthFunction(render::CompareMode);

			render::CompareMode
			depthFunction() const;

			Ptr
			triangleCulling(render::TriangleCulling);

			render::TriangleCulling
			triangleCulling() const;

			Ptr
			stencilFunction(render::CompareMode);

			render::CompareMode
			stencilFunction() const;

			Ptr
			stencilReference(int);

			int
			stencilReference() const;

			Ptr
			stencilMask(uint);

			uint
			stencilMask() const;

			Ptr
			stencilFailOperation(render::StencilOperation);

			render::StencilOperation
			stencilFailOperation() const;

			Ptr
			stencilDepthFailOperation(render::StencilOperation);

			render::StencilOperation
			stencilDepthFailOperation() const;

			Ptr
			stencilDepthPassOperation(render::StencilOperation);

			render::StencilOperation
			stencilDepthPassOperation() const;

			Ptr
			priority(float);

			float 
			priority() const;

			Ptr
			zSorted(bool);

			bool
			zSorted() const;

			Ptr
			isTransparent(bool transparent, bool zSort = false);

			Ptr
			target(AbsTexturePtr);

			AbsTexturePtr
			target() const;

		protected:
			BasicMaterial();

			virtual
			void
			initialize();
		};
	}
}
