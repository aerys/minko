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

#pragma once

#include "minko/Common.hpp"

#include "minko/render/Blending.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/StencilOperation.hpp"
#include "minko/render/TextureSampler.hpp"
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
            static const std::string                    PROPERTY_DEPTH_FUNCTION;
            static const std::string                    PROPERTY_TRIANGLE_CULLING;
            static const std::string                    PROPERTY_STENCIL_FUNCTION;
            static const std::string                    PROPERTY_STENCIL_REFERENCE;
            static const std::string                    PROPERTY_STENCIL_MASK;
            static const std::string                    PROPERTY_STENCIL_FAIL_OPERATION;
            static const std::string                    PROPERTY_STENCIL_ZFAIL_OPERATION;
            static const std::string                    PROPERTY_STENCIL_ZPASS_OPERATION;
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
            static const CompareMode		            DEFAULT_DEPTH_FUNCTION;
            static const TriangleCulling                DEFAULT_TRIANGLE_CULLING;
            static const CompareMode			        DEFAULT_STENCIL_FUNCTION;
            static const int					        DEFAULT_STENCIL_REFERENCE;
            static const uint					        DEFAULT_STENCIL_MASK;
            static const StencilOperation		        DEFAULT_STENCIL_FAIL_OPERATION;
            static const StencilOperation		        DEFAULT_STENCIL_ZFAIL_OPERATION;
            static const StencilOperation		        DEFAULT_STENCIL_ZPASS_OPERATION;
            static const bool					        DEFAULT_SCISSOR_TEST;
            static const math::ivec4 			        DEFAULT_SCISSOR_BOX;
            static const TextureSampler                 DEFAULT_TARGET;

        private:
            std::shared_ptr<data::Provider> _data;

		public:
            States(float					priority            = DEFAULT_PRIORITY,
                   bool						zSorted             = DEFAULT_ZSORTED,
                   Blending::Source			blendingSource      = DEFAULT_BLENDING_SOURCE,
                   Blending::Destination	blendingDestination = DEFAULT_BLENDING_DESTINATION,
                   bool						colorMask           = DEFAULT_COLOR_MASK,
                   bool						depthMask           = DEFAULT_DEPTH_MASK,
                   CompareMode				depthFunc           = DEFAULT_DEPTH_FUNCTION,
                   TriangleCulling          triangleCulling     = DEFAULT_TRIANGLE_CULLING,
                   CompareMode				stencilFunction     = DEFAULT_STENCIL_FUNCTION,
                   int						stencilRef          = DEFAULT_STENCIL_REFERENCE,
                   uint						stencilMask         = DEFAULT_STENCIL_MASK,
                   StencilOperation			stencilFailOp       = DEFAULT_STENCIL_FAIL_OPERATION,
                   StencilOperation			stencilZFailOp      = DEFAULT_STENCIL_ZFAIL_OPERATION,
                   StencilOperation			stencilZPassOp      = DEFAULT_STENCIL_ZPASS_OPERATION,
                   bool						scissorTest         = DEFAULT_SCISSOR_TEST,
                   const math::ivec4&		scissorBox          = DEFAULT_SCISSOR_BOX,
                   const TextureSampler&	target              = DEFAULT_TARGET);

            States(const States& states);

            States(std::shared_ptr<data::Provider> data);

            void
            resetDefaultValues();

            inline
            std::shared_ptr<data::Provider>
            data()
            {
                return _data;
            }

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
            States&
            blendingSourceFactor(Blending::Source value)
            {
                _data->set(PROPERTY_BLENDING_SOURCE, value);

                return *this;
            }

            inline
            Blending::Destination
            blendingDestinationFactor() const
            {
                return _data->get<Blending::Destination>(PROPERTY_BLENDING_DESTINATION);
            }

            inline
            States&
            blendingDestinationFactor(Blending::Destination value)
            {
                _data->set(PROPERTY_BLENDING_DESTINATION, value);

                return *this;
            }

			inline
			bool
			colorMask() const
			{
				return _data->get<bool>(PROPERTY_COLOR_MASK);
			}

            inline
            States&
            colorMask(bool value)
            {
                _data->set(PROPERTY_COLOR_MASK, value);

                return *this;
            }

            inline
            bool
            depthMask() const
            {
                return _data->get<bool>(PROPERTY_DEPTH_MASK);
            }

            inline
            States&
            depthMask(bool value)
            {
                _data->set(PROPERTY_DEPTH_MASK, value);

                return *this;
            }

            inline
            CompareMode
            depthFunction() const
            {
                return _data->get<CompareMode>(PROPERTY_DEPTH_FUNCTION);
            }

            inline
            States&
            depthFunction(CompareMode value)
            {
                _data->set(PROPERTY_DEPTH_FUNCTION, value);

                return *this;
            }

            inline
            TriangleCulling
            triangleCulling() const
            {
                return _data->get<TriangleCulling>(PROPERTY_TRIANGLE_CULLING);
            }

            inline
            States&
            triangleCulling(TriangleCulling value)
            {
                _data->set(PROPERTY_TRIANGLE_CULLING, value);

                return *this;
            }

			inline
			CompareMode
			stencilFunction() const
			{
                return _data->get<CompareMode>(PROPERTY_STENCIL_FUNCTION);
			}

            inline
            States&
            stencilFunction(CompareMode value)
            {
                _data->set(PROPERTY_STENCIL_FUNCTION, value);

                return *this;
            }

			inline
			int
			stencilReference() const
			{
				return _data->get<int>(PROPERTY_STENCIL_REFERENCE);
			}

            inline
            States&
            stencilReference(int value)
            {
                _data->set(PROPERTY_STENCIL_REFERENCE, value);

                return *this;
            }

			inline
			uint
			stencilMask() const
			{
				return _data->get<uint>(PROPERTY_STENCIL_MASK);
			}

            inline
            States&
            stencilMask(uint value)
            {
                _data->set(PROPERTY_STENCIL_MASK, value);

                return *this;
            }

			inline
			StencilOperation
			stencilFailOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_FAIL_OPERATION);
			}

            inline
            States&
            stencilFailOperation(StencilOperation value)
            {
                _data->set(PROPERTY_STENCIL_FAIL_OPERATION, value);

                return *this;
            }

			inline
			StencilOperation
			stencilZFailOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_ZFAIL_OPERATION);
			}

            inline
            States&
            stencilZFailOperation(StencilOperation value)
            {
                _data->set(PROPERTY_STENCIL_ZFAIL_OPERATION, value);

                return *this;
            }

			inline
			StencilOperation
			stencilZPassOperation() const
			{
                return _data->get<StencilOperation>(PROPERTY_STENCIL_ZPASS_OPERATION);
			}

            inline
            States&
            stencilZPassOperation(StencilOperation value)
            {
                _data->set(PROPERTY_STENCIL_ZPASS_OPERATION, value);

                return *this;
            }

			inline
			bool
			scissorTest() const
			{
                return _data->get<bool>(PROPERTY_SCISSOR_TEST);
			}

            inline
            States&
            scissorTest(bool value)
            {
                _data->set(PROPERTY_SCISSOR_TEST, value);

                return *this;
            }

			inline
			math::ivec4
			scissorBox() const
			{
                return _data->get<math::ivec4>(PROPERTY_SCISSOR_BOX);
			}

            inline
            States&
            scissorBox(const math::ivec4& value)
            {
                _data->set(PROPERTY_SCISSOR_BOX, value);

                return *this;
            }

            inline
            const TextureSampler&
            target() const
            {
                return _data->get<TextureSampler>(PROPERTY_TARGET);
            }

            inline
            States&
            target(const TextureSampler& target)
            {
                _data->set(PROPERTY_TARGET, target);

                return *this;
            }
        };
	}
}
