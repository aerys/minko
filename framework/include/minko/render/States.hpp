/*
Copyright (c) 2022 Aerys

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
#include "minko/render/Priority.hpp"

namespace minko
{
    namespace render
    {
	    class States
	    {
        private:
            typedef std::shared_ptr<render::AbstractTexture>    AbsTexturePtr;

        public:
            inline static
            const std::string&
            priorityPropertyName()
            {
                static const std::string p = "priority";
                return p;
            };
            
            inline static
            const std::string&
            zSortedPropertyName()
            {
                static const std::string p = "zSorted";
                return p;
            };
            
            inline static
            const std::string&
            blendingSourcePropertyName()
            {
                static const std::string p = "blendingSource";
                return p;
            };
            
            inline static
            const std::string&
            blendingDestinationPropertyName()
            {
                static const std::string p = "blendingDestination";
                return p;
            };
            
            inline static
            const std::string&
            colorMaskPropertyName()
            {
                static const std::string p = "colorMask";
                return p;
            };
            
            inline static
            const std::string&
            depthMaskPropertyName()
            {
                static const std::string p = "depthMask";
                return p;
            };
            
            inline static
            const std::string&
            depthFunctionPropertyName()
            {
                static const std::string p = "depthFunction";
                return p;
            };
            
            inline static
            const std::string&
            triangleCullingPropertyName()
            {
                static const std::string p = "triangleCulling";
                return p;
            };
            
            inline static
            const std::string&
            stencilFunctionPropertyName()
            {
                static const std::string p = "stencilFunction";
                return p;
            };
            
            inline static
            const std::string&
            stencilReferencePropertyName()
            {
                static const std::string p = "stencilReference";
                return p;
            };
            
            inline static
            const std::string&
            stencilMaskPropertyName()
            {
                static const std::string p = "stencilMask";
                return p;
            };
            
            inline static
            const std::string&
            stencilFailOperationPropertyName()
            {
                static const std::string p = "stencilFailOperation";
                return p;
            };
            
            inline static
            const std::string&
            stencilZFailOperationPropertyName()
            {
                static const std::string p = "stencilZFailOperation";
                return p;
            };
            
            inline static
            const std::string&
            stencilZPassOperationPropertyName()
            {
                static const std::string p = "stencilZPassOperation";
                return p;
            };
            
            inline static
            const std::string&
            scissorTestPropertyName()
            {
                static const std::string p = "scissorTest";
                return p;
            };
            
            inline static
            const std::string&
            scissorBoxPropertyName()
            {
                static const std::string p = "scissorBox";
                return p;
            };
            
            inline static
            const std::string&
            targetPropertyName()
            {
                static const std::string p = "target";
                return p;
            };

            static std::array<std::string, 17> const&   propertyNames();

            inline static
            const float&
            priorityDefaultValue()
            {
                static float v = render::Priority::OPAQUE;
                return v;
            }

            inline static
            const bool&
            zSortedDefaultValue()
            {
                static bool v = false;
                return v;
            }

            inline static
            const Blending::Source&
            blendingSourceDefaultValue()
            {
                static Blending::Source v = render::Blending::Source::ONE;
                return v;
            }

            inline static
            const Blending::Destination&
            blendingDestinationDefaultValue()
            {
                static Blending::Destination v = render::Blending::Destination::ZERO;
                return v;
            }

            inline static
            const bool&
            colorMaskDefaultValue()
            {
                static bool v = true;
                return v;
            }

            inline static
            const bool&
            depthMaskDefaultValue()
            {
                static bool v = true;
                return v;
            }

            inline static
            const CompareMode&
            depthFunctionDefaultValue()
            {
                static CompareMode v = render::CompareMode::LESS;
                return v;
            }

            inline static
            const TriangleCulling&
            triangleCullingDefaultValue()
            {
                static TriangleCulling v = render::TriangleCulling::BACK;
                return v;
            }

            inline static
            const CompareMode&
            stencilFunctionDefaultValue()
            {
                static CompareMode v = render::CompareMode::ALWAYS;
                return v;
            }

            inline static
            const int&
            stencilReferenceDefaultValue()
            {
                static int v = 0;
                return v;
            }

            inline static
            const uint&
            stencilMaskDefaultValue()
            {
                static uint v = 1;
                return v;
            }

            inline static
            const StencilOperation&
            stencilFailOperationDefaultValue()
            {
                static StencilOperation v = render::StencilOperation::KEEP;
                return v;
            }

            inline static
            const StencilOperation&
            stencilZFailOperationDefaultValue()
            {
                static StencilOperation v = render::StencilOperation::KEEP;
                return v;
            }

            inline static
            const StencilOperation&
            stencilZPassOperationDefaultValue()
            {
                static StencilOperation v = render::StencilOperation::KEEP;
                return v;
            }

            inline static
            const bool&
            scissorTestDefaultValue()
            {
                static bool v = false;
                return v;
            }

            inline static
            const math::ivec4&
            scissorBoxDefaultValue()
            {
                static math::ivec4 v(0);
                return v;
            }

            inline static
            const TextureSampler&
            targetDefaultValue()
            {
                static TextureSampler v("", nullptr);
                return v;
            }

        private:
            std::shared_ptr<data::Provider> _data;

		public:
            States();

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
                return _data->get<float>(priorityPropertyName());
            }

            inline
            States&
            priority(float priority)
            {
            	_data->set<float>(priorityPropertyName(), priority);

                return *this;
            }

			inline
			bool
			zSorted() const
			{
				return _data->get<bool>(zSortedPropertyName());
			}

            inline
            States&
            zSorted(bool zSorted)
            {
                _data->set(zSortedPropertyName(), zSorted);

                return *this;
            }

            inline
            Blending::Source
            blendingSource() const
            {
                return _data->get<Blending::Source>(blendingSourcePropertyName());
            }

            inline
            States&
            blendingSource(Blending::Source value)
            {
                _data->set(blendingSourcePropertyName(), value);

                return *this;
            }

            inline
            Blending::Destination
            blendingDestination() const
            {
                return _data->get<Blending::Destination>(blendingDestinationPropertyName());
            }

            inline
            States&
            blendingDestination(Blending::Destination value)
            {
                _data->set(blendingDestinationPropertyName(), value);

                return *this;
            }

			inline
			bool
			colorMask() const
			{
				return _data->get<bool>(colorMaskPropertyName());
			}

            inline
            States&
            colorMask(bool value)
            {
                _data->set(colorMaskPropertyName(), value);

                return *this;
            }

            inline
            bool
            depthMask() const
            {
                return _data->get<bool>(depthMaskPropertyName());
            }

            inline
            States&
            depthMask(bool value)
            {
                _data->set(depthMaskPropertyName(), value);

                return *this;
            }

            inline
            CompareMode
            depthFunction() const
            {
                return _data->get<CompareMode>(depthFunctionPropertyName());
            }

            inline
            States&
            depthFunction(CompareMode value)
            {
                _data->set(depthFunctionPropertyName(), value);

                return *this;
            }

            inline
            TriangleCulling
            triangleCulling() const
            {
                return _data->get<TriangleCulling>(triangleCullingPropertyName());
            }

            inline
            States&
            triangleCulling(TriangleCulling value)
            {
                _data->set(triangleCullingPropertyName(), value);

                return *this;
            }

			inline
			CompareMode
			stencilFunction() const
			{
                return _data->get<CompareMode>(stencilFunctionPropertyName());
			}

            inline
            States&
            stencilFunction(CompareMode value)
            {
                _data->set(stencilFunctionPropertyName(), value);

                return *this;
            }

			inline
			int
			stencilReference() const
			{
				return _data->get<int>(stencilReferencePropertyName());
			}

            inline
            States&
            stencilReference(int value)
            {
                _data->set(stencilReferencePropertyName(), value);

                return *this;
            }

			inline
			uint
			stencilMask() const
			{
				return _data->get<uint>(stencilMaskPropertyName());
			}

            inline
            States&
            stencilMask(uint value)
            {
                _data->set(stencilMaskPropertyName(), value);

                return *this;
            }

			inline
			StencilOperation
			stencilFailOperation() const
			{
                return _data->get<StencilOperation>(stencilFailOperationPropertyName());
			}

            inline
            States&
            stencilFailOperation(StencilOperation value)
            {
                _data->set(stencilFailOperationPropertyName(), value);

                return *this;
            }

			inline
			StencilOperation
			stencilZFailOperation() const
			{
                return _data->get<StencilOperation>(stencilZFailOperationPropertyName());
			}

            inline
            States&
            stencilZFailOperation(StencilOperation value)
            {
                _data->set(stencilZFailOperationPropertyName(), value);

                return *this;
            }

			inline
			StencilOperation
			stencilZPassOperation() const
			{
                return _data->get<StencilOperation>(stencilZPassOperationPropertyName());
			}

            inline
            States&
            stencilZPassOperation(StencilOperation value)
            {
                _data->set(stencilZPassOperationPropertyName(), value);

                return *this;
            }

			inline
			bool
			scissorTest() const
			{
                return _data->get<bool>(scissorTestPropertyName());
			}

            inline
            States&
            scissorTest(bool value)
            {
                _data->set(scissorTestPropertyName(), value);

                return *this;
            }

			inline
			math::ivec4
			scissorBox() const
			{
                return _data->get<math::ivec4>(scissorBoxPropertyName());
			}

            inline
            States&
            scissorBox(const math::ivec4& value)
            {
                _data->set(scissorBoxPropertyName(), value);

                return *this;
            }

            inline
            const TextureSampler&
            target() const
            {
                return _data->get<TextureSampler>(targetPropertyName());
            }

            inline
            States&
            target(const TextureSampler& target)
            {
                _data->set(targetPropertyName(), target);

                return *this;
            }
        };
	}
}
