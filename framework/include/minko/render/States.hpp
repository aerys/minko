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

#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/data/Store.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
    namespace render
    {
	    class States
	    {
        private:
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;

        public:
            static const std::string                    PROPERTY_PRIORITY;
            static const std::string                    PROPERTY_ZSORTED;
            static const std::string                    PROPERTY_BLENDING_SOURCE;
            static const std::string                    PROPERTY_BLENDING_DESTINATION;
            static const std::string                    PROPERTY_COLOR_MASK;
            static const std::string                    PROPERTY_DEPTH_MASK;
            static const std::string                    PROPERTY_DEPTH_FUNC;
            static const std::string                    PROPERTY_TRIANGLE_CULLING;
            static const std::string                    PROPERTY_STENCIL_FUNCTION;
            static const std::string                    PROPERTY_STENCIL_REFERENCE;
            static const std::string                    PROPERTY_STENCIL_MASK;
            static const std::string                    PROPERTY_STENCIL_FAIL_OP;
            static const std::string                    PROPERTY_STENCIL_ZFAIL_OP;
            static const std::string                    PROPERTY_STENCIL_ZPASS_OP;
            static const std::string                    PROPERTY_SCISSOR_TEST;
            static const std::string                    PROPERTY_SCISSOR_BOX;
            static const std::string                    PROPERTY_TARGET;

            static const std::array<std::string, 17>    PROPERTY_NAMES;

            static const float				            DEFAULT_PRIORITY;
            static const bool					        DEFAULT_ZSORTED;
            static const Blending::Source		        DEFAULT_BLENDING_SOURCE;
            static const Blending::Destination	        DEFAULT_BLENDING_DESTINATION;
            static const bool					        DEFAULT_COLOR_MASK;
            static const bool					        DEFAULT_DEPTH_MASK;
            static const CompareMode		            DEFAULT_DEPTH_FUNC;
            static const TriangleCulling                DEFAULT_TRIANGLE_CULLING;
            static const CompareMode			        DEFAULT_STENCIL_FUNCTION;
            static const int					        DEFAULT_STENCIL_REFERENCE;
            static const uint					        DEFAULT_STENCIL_MASK;
            static const StencilOperation		        DEFAULT_STENCIL_FAIL_OP;
            static const StencilOperation		        DEFAULT_STENCIL_ZFAIL_OP;
            static const StencilOperation		        DEFAULT_STENCIL_ZPASS_OP;
            static const bool					        DEFAULT_SCISSOR_TEST;
            static const math::ivec4 			        DEFAULT_SCISSOR_BOX;
            static const AbsTexturePtr                  DEFAULT_TARGET;

        private:
            std::shared_ptr<data::Provider> _data;
            data::Store                 _container;

		public:
            States(float					priority            = DEFAULT_PRIORITY,
                   bool						zSorted             = DEFAULT_ZSORTED,
                   Blending::Source			blendingSource      = DEFAULT_BLENDING_SOURCE,
                   Blending::Destination	blendingDestination = DEFAULT_BLENDING_DESTINATION,
                   bool						colorMask           = DEFAULT_COLOR_MASK,
                   bool						depthMask           = DEFAULT_DEPTH_MASK,
                   CompareMode				depthFunc           = DEFAULT_DEPTH_FUNC,
                   TriangleCulling          triangleCulling     = DEFAULT_TRIANGLE_CULLING,
                   CompareMode				stencilFunction     = DEFAULT_STENCIL_FUNCTION,
                   int						stencilRef          = DEFAULT_STENCIL_REFERENCE,
                   uint						stencilMask         = DEFAULT_STENCIL_MASK,
                   StencilOperation			stencilFailOp       = DEFAULT_STENCIL_FAIL_OP,
                   StencilOperation			stencilZFailOp      = DEFAULT_STENCIL_ZFAIL_OP,
                   StencilOperation			stencilZPassOp      = DEFAULT_STENCIL_ZPASS_OP,
                   bool						scissorTest         = DEFAULT_SCISSOR_TEST,
                   const math::ivec4&		scissorBox          = DEFAULT_SCISSOR_BOX,
                   AbsTexturePtr		    target              = DEFAULT_TARGET);

            States(const States& states);

            void
            resetDefaultValues();

            inline
            float
            priority() const
            {
                return _data->get<float>(PROPERTY_PRIORITY);
            }
            inline
            States&
            priority(float priority)
            {
            	_data->set<float>(PROPERTY_PRIORITY, priority);

                return *this;
            }

			inline
			bool
			zSorted() const
			{
				return _data->get<bool>(PROPERTY_ZSORTED);
			}
            inline
            States&
            zSorted(bool zSorted)
            {
                _data->set(PROPERTY_ZSORTED, zSorted);

                return *this;
            }

            inline
            Blending::Source
            blendingSourceFactor() const
            {
                return _data->get<Blending::Source>(PROPERTY_BLENDING_SOURCE);
            }

            inline
            Blending::Destination
            blendingDestinationFactor() const
            {
                return _data->get<Blending::Destination>(PROPERTY_BLENDING_DESTINATION);
            }

			inline
			bool
			colorMask() const
			{
				return _data->get<bool>(PROPERTY_COLOR_MASK);
			}

            inline
            bool
            depthMask() const
            {
                return _data->get<bool>(PROPERTY_DEPTH_MASK);
            }

            inline
            CompareMode
            depthFunc() const
            {
                return _data->get<CompareMode>(PROPERTY_DEPTH_FUNC);
            }

            inline
            TriangleCulling
            triangleCulling() const
            {
                return _data->get<TriangleCulling>(PROPERTY_TRIANGLE_CULLING);
            }

			inline
			CompareMode
			stencilFunction() const
			{
                return _data->get<CompareMode>(PROPERTY_STENCIL_FUNCTION);
			}

			inline
			int
			stencilReference() const
			{
				return _data->get<int>(PROPERTY_STENCIL_REFERENCE);
			}

			inline
			uint
			stencilMask() const
			{
				return _data->get<uint>(PROPERTY_STENCIL_MASK);
			}

			inline
			StencilOperation
			stencilFailOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_FAIL_OP);
			}

			inline
			StencilOperation
			stencilDepthFailOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_ZFAIL_OP);
			}

			inline
			StencilOperation
			stencilDepthPassOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_ZPASS_OP);
			}

			inline
			bool
			scissorTest() const
			{
                return _data->get<bool>(PROPERTY_SCISSOR_TEST);
			}

			inline
			math::ivec4
			scissorBox() const
			{
                return _data->get<math::ivec4>(PROPERTY_SCISSOR_BOX);
			}

            /*inline
            AbstractTexturePtr
            target() const
            {
                return _data->get<>(PROPERTY_TARGET);
            }*/
        };
	}
}
