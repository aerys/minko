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

#include "minko/component/AbstractComponent.hpp"
#include "minko/Signal.hpp"

namespace minko
{
	namespace component
	{
		class LightManager :
			public AbstractComponent,
			public std::enable_shared_from_this<LightManager>
		{
		public:
			typedef std::shared_ptr<LightManager>	Ptr;

		private:
			typedef std::shared_ptr<AbstractComponent>	AbsCmpPtr;
			typedef std::shared_ptr<scene::Node>		NodePtr;

		private:
			std::shared_ptr<data::Provider>				_data;
			std::set<std::shared_ptr<AmbientLight>>		_ambientLights;
			std::set<std::shared_ptr<DirectionalLight>>	_directionalLights;
			std::set<std::shared_ptr<PointLight>>		_pointLights;
			std::set<std::shared_ptr<SpotLight>>		_spotLights;

			std::shared_ptr<math::Vector3>				_ambientLightsContribution;

			Signal<AbsCmpPtr, NodePtr>::Slot			_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot			_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 	_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 	_removedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot 	_componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot 	_componentRemovedSlot;

		public:
			inline static
			Ptr
			create()
			{
				auto lm = std::shared_ptr<LightManager>(new LightManager());

				lm->initialize();

				return lm;
			}

			~LightManager()
			{
			}

			void
			initialize();

		private:
			LightManager();

			void
			targetAddedHandler(AbsCmpPtr cmp, NodePtr target);

			void
			targetRemovedHandler(AbsCmpPtr cmp, NodePtr target);

			void
			addedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

			void
			componentAddedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp);

			void
			componentRemovedHandler(NodePtr node, NodePtr target, AbsCmpPtr cmp);

			void
			setLightArrayLength(const std::string& arrayName, int length);

			void
			setLightColor(const std::string& arrayName, std::shared_ptr<math::Vector3> color);
		};
	}
}
