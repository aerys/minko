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

#include "minko/data/ContainerProperty.hpp"

namespace minko
{
	namespace component
	{
		class Surface :
			public AbstractComponent,
			public std::enable_shared_from_this<Surface>
		{
			friend render::DrawCallPool;

		public:
			typedef std::shared_ptr<Surface>						Ptr;
			typedef Signal<Ptr, const std::string&, bool>			TechniqueChangedSignal;	
			typedef Signal<Ptr, bool>								VisibilityChangedSignal;

		private:
			typedef std::shared_ptr<data::ArrayProvider>					ArrayProviderPtr;
			typedef std::shared_ptr<scene::Node>							NodePtr;
			typedef std::shared_ptr<data::Container>						ContainerPtr;
			typedef Signal<ContainerPtr, const std::string&>				PropertyChangedSignal;
			typedef PropertyChangedSignal::Slot								PropertyChangedSlot;
			typedef std::shared_ptr<render::Effect>							EffectPtr;
			typedef const std::string&										StringRef;
			typedef Signal<ArrayProviderPtr, uint>::Slot					ArrayProviderIndexChangedSlot;


		private:
			std::shared_ptr<geometry::Geometry>										_geometry;
			std::shared_ptr<data::Provider>											_material;
			std::shared_ptr<render::Effect>											_effect;
			
			std::string 															_technique;
			std::list<ArrayProviderIndexChangedSlot>								_dataProviderIndexChangedSlots;

			int																		_geometryId;
			int																		_materialId;
			
			TechniqueChangedSignal::Ptr												_techniqueChanged;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot									_removedSlot;
			Signal<EffectPtr, StringRef, StringRef>::Slot							_techniqueChangedSlot;

			bool																	_visible;
			bool																	_computedVisibility;

			VisibilityChangedSignal::Ptr											_visibilityChanged;
			VisibilityChangedSignal::Ptr											_computedVisibilityChanged;

		public:
			static
			Ptr
			create(std::shared_ptr<geometry::Geometry> 		geometry,
				   std::shared_ptr<data::Provider>			material,
				   std::shared_ptr<render::Effect>			effect)
			{
				return create(geometry, material, effect, "default");
			}

			static
			Ptr
			create(std::shared_ptr<geometry::Geometry> 		geometry,
				   std::shared_ptr<data::Provider>			material,
				   std::shared_ptr<render::Effect>			effect,
				   const std::string&						technique)
			{
				Ptr surface(new Surface(geometry, material, effect, technique));

				surface->initialize();

				return surface;
			}

			~Surface()
			{
			}

			inline
			bool
			visible()
			{
				return _visible;
			}

			inline
			VisibilityChangedSignal::Ptr
			visibilityChanged()
			{
				return _visibilityChanged;
			}

			void
			visible(bool value);

			inline
			bool
			computedVisibility()
			{
				return _computedVisibility;
			}
			
			void
			computedVisibility(bool value);

			inline
			VisibilityChangedSignal::Ptr
			computedVisibilityChanged()
			{
				return _computedVisibilityChanged;
			}

			inline
			std::shared_ptr<geometry::Geometry>
			geometry()
			{
				return _geometry;
			}

            void
            geometry(std::shared_ptr<geometry::Geometry> newGeometry);

			inline
			std::shared_ptr<data::Provider>
			material()
			{
				return _material;
			}

			inline
			std::shared_ptr<render::Effect>
			effect()
			{
				return _effect;
			}

			inline
			const std::string&
			technique()
			{
				return _technique;
			}

			inline
			TechniqueChangedSignal::Ptr	
			techniqueChanged() const
			{
				return _techniqueChanged;
			}

		private:
			Surface(std::shared_ptr<geometry::Geometry>		geometry,
					std::shared_ptr<data::Provider>			material,
					std::shared_ptr<render::Effect>			effect,
					const std::string&						technique);

			void
			initialize();

			void
			targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

			void
			setTechnique(const std::string&, bool updateDrawcalls = true);

			void
			geometryProviderIndexChanged(ArrayProviderPtr arrayProvider, uint index);

			void
			materialProviderIndexChanged(ArrayProviderPtr arrayProvider, uint index);
		};
	}
}