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

#include "minko/component/AbstractLight.hpp"
#include "minko/Flyweight.hpp"
#include "minko/Signal.hpp"

namespace minko
{
	namespace component
	{
		class AbstractDiscreteLight :
			public AbstractLight
		{
        private:
            typedef std::shared_ptr<data::Provider>     ProviderPtr;
            typedef const Flyweight<std::string>&       PropertyName;

		private:
			Signal<data::Store&, ProviderPtr, PropertyName>::Slot	_modelToWorldChangedSlot;

		public:
			inline
		    float
		    diffuse() const
		    {
		    	return data()->get<float>("diffuse");
		    }

		    inline
		    Ptr
		    diffuse(float diffuse)
		    {
		    	data()->set<float>("diffuse", diffuse);

				return std::static_pointer_cast<AbstractDiscreteLight>(shared_from_this());
		    }

			inline
			float
			specular() const
			{
				return data()->get<float>("specular");
			}

			inline
			Ptr
			specular(float specular)
			{
				data()->set<float>("specular", specular);

				return std::static_pointer_cast<AbstractDiscreteLight>(shared_from_this());
			}

		protected:
			AbstractDiscreteLight(const std::string&	arrayName,
								  float					diffuse		= 1.0f,
								  float					specular	= 1.0f);

			virtual
            void
            targetAdded(std::shared_ptr<scene::Node> target);

            virtual
            void
            targetRemoved(std::shared_ptr<scene::Node> target);

            void
            modelToWorldMatrixChangedHandler(data::Store& 					container,
            								 const Flyweight<std::string>& 	propertyName);

            virtual
            void
            updateModelToWorldMatrix(const math::mat4& modelToWorld) = 0;
		};
	}
}
