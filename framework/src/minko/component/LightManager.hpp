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

#include "minko/component/AbstractRootDataComponent.hpp"
#include "minko/Signal.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/AmbientLight.hpp"
#include "minko/component/DirectionalLight.hpp"

namespace minko
{
	namespace component
	{
		class LightManager :
			public AbstractRootDataComponent
		{
		public:
			typedef std::shared_ptr<LightManager>	Ptr;

		private:
			typedef std::shared_ptr<AbstractComponent>	AbsCmpPtr;
			typedef std::shared_ptr<scene::Node>		NodePtr;

		private:
			int										_numAmbientLights;
			int										_numDirectionalLights;

			Signal<NodePtr, NodePtr, NodePtr>::Slot _addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot _removedSlot;

		public:
			inline static
			Ptr
			create()
			{
				auto lm = std::shared_ptr<LightManager>(new LightManager());

				lm->initialize();

				return lm;
			}

		private:
			LightManager() :
				AbstractRootDataComponent(),
				_numAmbientLights(0),
				_numDirectionalLights(0),
				_addedSlot(nullptr),
				_removedSlot(nullptr)
			{
				data()
					->set("ambientLights.length", 		0)
					->set("directionalLights.length", 	0);
			}

			void
			targetAddedHandler(AbsCmpPtr cmp, NodePtr target)
			{
				AbstractRootDataComponent::targetAddedHandler(cmp, target);

				if (target->components<LightManager>().size() > 1)
					throw std::logic_error("There cannot be more than one LightManager.");
				if (target->root() != target)
					throw std::invalid_argument("target");

				_addedSlot = target->added()->connect(std::bind(
					&LightManager::addedHandler,
					std::dynamic_pointer_cast<LightManager>(shared_from_this()),
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				));

				_removedSlot = target->removed()->connect(std::bind(
					&LightManager::removedHandler,
					std::dynamic_pointer_cast<LightManager>(shared_from_this()),
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3
				));
			}

			void
			targetRemovedHandler(AbsCmpPtr cmp, NodePtr target)
			{
				AbstractRootDataComponent::targetRemovedHandler(cmp, target);

				_addedSlot = nullptr;
				_removedSlot = nullptr;
			}

			void
			addedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
			{
				if (target->children().size() == 0)
				{
					_numAmbientLights += target->components<AmbientLight>().size();
					_numDirectionalLights += target->components<DirectionalLight>().size();					
				}
				else
				{
					for (auto& descendant : scene::NodeSet::create(target)->descendants(true)->nodes())
					{
						_numAmbientLights += descendant->components<AmbientLight>().size();
						_numDirectionalLights += descendant->components<DirectionalLight>().size();
					}
				}

				data()
					->set("ambientLights.length", 		_numAmbientLights)
					->set("directionalLights.length", 	_numDirectionalLights);
			}

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
			{
				if (target->children().size() == 0)
				{
					_numAmbientLights -= target->components<AmbientLight>().size();
					_numDirectionalLights -= target->components<DirectionalLight>().size();					
				}
				else
				{
					for (auto& descendant : scene::NodeSet::create(target)->descendants(true)->nodes())
					{
						_numAmbientLights -= descendant->components<AmbientLight>().size();
						_numDirectionalLights -= descendant->components<DirectionalLight>().size();
					}
				}

				data()
					->set("ambientLights.length", 		_numAmbientLights)
					->set("directionalLights.length", 	_numDirectionalLights);
			}

			void
			sortLights()
			{
				// FIXME
			}
		};
	}
}
