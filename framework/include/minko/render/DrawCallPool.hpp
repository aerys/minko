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
#include "minko/Signal.hpp"
#include "minko/data/ContainerProperty.hpp"

namespace minko
{
	namespace render
	{
		class DrawCallPool :
			public std::enable_shared_from_this<DrawCallPool>
		{
		public:
			typedef std::shared_ptr<DrawCallPool>						Ptr;
			typedef std::shared_ptr<DrawCall>							DrawCallPtr; 
			typedef std::shared_ptr<component::Renderer>				RendererPtr;
			typedef std::shared_ptr<component::Surface>					SurfacePtr;
			typedef std::list<DrawCallPtr>								DrawCallList;
			typedef std::shared_ptr<data::Container>					ContainerPtr;

			typedef Signal<ContainerPtr, const std::string&>			PropertyChangedSignal;
			typedef Signal<SurfacePtr, const std::string&, bool>::Slot	TechniqueChangeSlot;
			typedef Signal<SurfacePtr, bool>::Slot						VisibilityChangedSlot;
			typedef Signal<DrawCallPtr>::Slot							ZSortNeededSlot;
			typedef PropertyChangedSignal::Slot							PropertyChangedSlot;
			typedef std::shared_ptr<render::Pass>						PassPtr;
			

			typedef std::pair<std::string, PassPtr>						TechniquePass;

		private:
			typedef std::shared_ptr<component::Renderer>									RendererPtr;
			typedef Signal<Ptr, SurfacePtr, DrawCallPtr>									DrawCallChangedSignal;
			typedef std::unordered_map<DrawCallPtr, PassPtr>								DrawCallToPassMap;
			typedef Signal<std::shared_ptr<data::ArrayProvider>, uint>::Slot				ArrayProviderIndexChangedSlot;
			typedef std::unordered_map<data::ContainerProperty, PropertyChangedSlot>		ContainerPropertyToChangedSlotMap;
			typedef std::unordered_map<data::ContainerProperty, uint>						ContainerPropertyToNumListenersMap;
			typedef std::unordered_map<data::ContainerProperty, std::list<TechniquePass>>	ContainerPropertyToPassesMap;
			typedef std::unordered_map<DrawCallPtr, ContainerPtr>							DrawCallToContainerMap;
			typedef std::unordered_map<std::string, DrawCallList>							MacroNameToDrawCallListMap;
			typedef std::unordered_map<std::string, std::unordered_set<std::string>>		TechniqueToMacroSetMap;
			typedef std::unordered_map<DrawCallPtr, ZSortNeededSlot>						DrawCallToZSortSlotMap;
			typedef std::unordered_map<std::string, data::MacroBinding>						MacroBindingsMap;

			enum class MacroChange
			{
				REF_CHANGED	= 0,
				ADDED		= 1,
				REMOVED		= 2
			};
			
		private:
			static const unsigned int NUM_FALLBACK_ATTEMPTS;
			static std::unordered_map<std::string, std::pair<std::string, int>>	_variablePropertyNameToPosition;

			RendererPtr																	_renderer;

			std::unordered_map<SurfacePtr, TechniqueChangeSlot>							_surfaceToTechniqueChangedSlot;
			std::unordered_multimap<SurfacePtr, VisibilityChangedSlot>					_surfaceToVisibilityChangedSlot;
			std::unordered_multimap<SurfacePtr, ArrayProviderIndexChangedSlot>			_surfaceToIndexChangedSlot;

			DrawCallChangedSignal::Ptr													_drawCallAdded;
			DrawCallChangedSignal::Ptr													_drawCallRemoved;

			std::unordered_map<SurfacePtr, std::list<Any>>								_macroAddedOrRemovedSlots;
			std::unordered_map<SurfacePtr, ContainerPropertyToChangedSlotMap>			_macroChangedSlots;
			std::unordered_map<SurfacePtr, ContainerPropertyToNumListenersMap>			_numMacroListeners;
			
			
			std::unordered_map<SurfacePtr, DrawCallToPassMap>							_drawCallToPass;
			std::unordered_map<SurfacePtr, DrawCallToContainerMap>						_drawCallToRendererData;
			std::unordered_map<SurfacePtr, MacroNameToDrawCallListMap>					_macroNameToDrawCalls;
			std::unordered_map<SurfacePtr, TechniqueToMacroSetMap>						_techniqueToMacroNames;

			std::unordered_map<SurfacePtr, ContainerPropertyToPassesMap>				_incorrectMacroToPasses;
			std::unordered_map<SurfacePtr, ContainerPropertyToChangedSlotMap>			_incorrectMacroChangedSlot;
			std::unordered_map<SurfacePtr, DrawCallToZSortSlotMap>						_drawcallToZSortNeededSlots;

			// surface that will generate new draw call next frame
			std::unordered_set<SurfacePtr>																			_toCollect;
			std::unordered_set<SurfacePtr>																			_toRemove;
			std::unordered_set<SurfacePtr>																			_invisibleSurfaces;
			bool																									_mustZSort;

			// draw call list for renderer
			std::unordered_map<SurfacePtr, DrawCallList>															_surfaceToDrawCalls;
			DrawCallList																							_drawCalls;


		public:
			inline static
			Ptr
			create(RendererPtr renderer)
			{
				return std::shared_ptr<DrawCallPool>(new DrawCallPool(renderer));
			}

			const std::list<std::shared_ptr<DrawCall>>&
			drawCalls();

			static
			bool
			compareDrawCalls(DrawCallPtr, DrawCallPtr);

			void
			addSurface(SurfacePtr);

			void
			removeSurface(SurfacePtr);
			
		private:
			
			DrawCallPool(RendererPtr renderer);

			void
			techniqueChanged(SurfacePtr surface, const std::string& technique, bool updateDrawCall);

			void
			visibilityChanged(SurfacePtr surface, bool visibility);

			// generate draw call for one mesh
			std::shared_ptr<DrawCall>
			initializeDrawCall(std::shared_ptr<render::Pass>	pass, 
							   SurfacePtr						surface,
							   std::shared_ptr<DrawCall>		drawcall = nullptr);

			std::shared_ptr<Program>
			getWorkingProgram(SurfacePtr							surface,
							  std::shared_ptr<Pass>					pass,
							  const MacroBindingsMap&				macroBindings,
						      std::shared_ptr<data::Container>		targetData,
						      std::shared_ptr<data::Container>		rendererData,
						      std::shared_ptr<data::Container>		rootData,
						      std::list<data::ContainerProperty>&	booleanMacros,
						      std::list<data::ContainerProperty>&	integerMacros,
						      std::list<data::ContainerProperty>&	incorrectIntegerMacros);
						
			DrawCallList&
			generateDrawCall(SurfacePtr, unsigned int numAttempts); 
			
			void
			deleteDrawCalls(SurfacePtr);

			void
			cleanSurface(SurfacePtr);

			void
			macroChangedHandler(ContainerPtr		container, 
							    const std::string&	propertyName, 
								SurfacePtr			surface, 
								MacroChange			change);

			
			void
			watchMacroAdditionOrDeletion(SurfacePtr);
			
			void
			blameMacros(SurfacePtr									surface,
						const std::list<data::ContainerProperty>&	incorrectIntegerMacros,
						const TechniquePass&						pass);
			
			void
			forgiveMacros(SurfacePtr									surface,
						  const std::list<data::ContainerProperty>&		booleanMacros,
						  const std::list<data::ContainerProperty>&		integerMacros,
						  const TechniquePass&							pass);
						  
			void
			incorrectMacroChangedHandler(SurfacePtr						surface,
										 const data::ContainerProperty& macro);

			void
			zsortNeededHandler(SurfacePtr, DrawCallPtr);

			void
			dataProviderIndexChanged(std::shared_ptr<data::ArrayProvider> provider, uint index, SurfacePtr surface);
		
			void
			replaceVariable(std::string& propertyName, std::unordered_map<std::string, std::string>& variableToValue);
		};
	}
}
