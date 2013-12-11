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
		public:
			typedef std::shared_ptr<Surface>						Ptr;
			typedef Signal<Ptr, std::shared_ptr<render::DrawCall>>	DrawCallChangedSignal;
			typedef Signal<Ptr, const std::string&, bool>			TechniqueChangedSignal;			

		private:
			typedef std::shared_ptr<scene::Node>					NodePtr;
			typedef std::shared_ptr<data::Container>				ContainerPtr;
			typedef std::shared_ptr<render::Pass>					PassPtr;
			typedef std::shared_ptr<render::DrawCall>				DrawCallPtr;
			typedef std::list<DrawCallPtr>							DrawCallList;
			typedef Signal<ContainerPtr, const std::string&>		PropertyChangedSignal;
			typedef PropertyChangedSignal::Slot						PropertyChangedSlot;
			typedef std::shared_ptr<render::Effect>					EffectPtr;
			typedef const std::string&								StringRef;
			typedef std::pair<std::string, PassPtr>					TechniquePass;
			enum class MacroChange
			{
				REF_CHANGED	= 0,
				ADDED		= 1,
				REMOVED		= 2
			};

		private:
			std::shared_ptr<geometry::Geometry>										_geometry;
			std::shared_ptr<data::Provider>											_material;
			std::shared_ptr<render::Effect>											_effect;
			std::unordered_map<std::string, std::unordered_set<std::string>>		_techniqueToMacroNames;

			std::string 															_technique;
			std::unordered_map<ContainerPtr, DrawCallList>							_drawCalls;
			std::unordered_map<DrawCallPtr, PassPtr>								_drawCallToPass;
			std::unordered_map<DrawCallPtr, ContainerPtr>							_drawCallToRendererData;
			std::unordered_map<std::string, DrawCallList>							_macroNameToDrawCalls;

			// current technique-related members
			std::list<Any>															_macroAddedOrRemovedSlots;
			std::unordered_map<data::ContainerProperty, PropertyChangedSlot>		_macroChangedSlots;
			std::unordered_map<data::ContainerProperty, uint>						_numMacroListeners;
			
			// cross-technique members
			std::unordered_map<data::ContainerProperty, std::list<TechniquePass>>	_incorrectMacroToPasses;
			std::unordered_map<data::ContainerProperty, PropertyChangedSlot>		_incorrectMacroChangedSlot;

			DrawCallChangedSignal::Ptr												_drawCallAdded;
			DrawCallChangedSignal::Ptr												_drawCallRemoved;
			TechniqueChangedSignal::Ptr												_techniqueChanged;

			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetAddedSlot;
			Signal<AbstractComponent::Ptr, NodePtr>::Slot							_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot									_removedSlot;
			Signal<EffectPtr, StringRef, StringRef>::Slot							_techniqueChangedSlot;

		public:
			static
			Ptr
			create(std::shared_ptr<geometry::Geometry> 	geometry,
				   std::shared_ptr<data::Provider>		material,
				   std::shared_ptr<render::Effect>		effect,
				   const std::string&					technique = "default")
			{
				Ptr surface(new Surface(geometry, material, effect, technique));

				surface->initialize();

				return surface;
			}

			~Surface()
			{
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
			DrawCallChangedSignal::Ptr
			drawCallAdded() const
			{
				return _drawCallAdded;
			}

			inline
			DrawCallChangedSignal::Ptr
			drawCallRemoved() const
			{
				return _drawCallRemoved;
			}

			const DrawCallList&
			createDrawCalls(std::shared_ptr<data::Container>	rendererData, unsigned int numAttempts);

			void
			deleteDrawCalls(std::shared_ptr<data::Container>	rendererData);

			inline
			TechniqueChangedSignal::Ptr	
			techniqueChanged() const
			{
				return _techniqueChanged;
			}

		private:
			Surface(std::shared_ptr<geometry::Geometry> geometry,
					std::shared_ptr<data::Provider>		material,
					std::shared_ptr<render::Effect>		effect,
					const std::string&					technique);

			void
			initialize();

			void
			initializeTechniqueMacroNames();

			std::shared_ptr<render::DrawCall>
			initializeDrawCall(std::shared_ptr<render::Pass>		pass,
							   std::shared_ptr<data::Container>		rendererData,
							   std::shared_ptr<render::DrawCall>	drawcall = nullptr);

			void
			targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

			void
			watchMacroAdditionOrDeletion(ContainerPtr rendererData);

			void
			macroChangedHandler(ContainerPtr, const std::string& propertyName, MacroChange);

			std::shared_ptr<render::Program>
			getWorkingProgram(std::shared_ptr<render::Pass>			pass,
							  ContainerPtr							targetData,
							  ContainerPtr							rendererData,
							  ContainerPtr							rootData,
							  std::list<data::ContainerProperty>&	booleanMacros,
							  std::list<data::ContainerProperty>&	integerMacros,
							  std::list<data::ContainerProperty>&	incorrectIntegerMacros);

			void
			setTechnique(const std::string&, bool updateDrawcalls = true);

			void
			forgiveMacros(const std::list<data::ContainerProperty>& booleanMacros,
						  const std::list<data::ContainerProperty>& integerMacros,
						  const TechniquePass&);

			void
			blameMacros(const std::list<data::ContainerProperty>& incorrectIntegerMacros,
						const TechniquePass&);

			void
			incorrectMacroChangedHandler(const data::ContainerProperty&);

			void
			deleteAllDrawCalls();
		};
	}
}
