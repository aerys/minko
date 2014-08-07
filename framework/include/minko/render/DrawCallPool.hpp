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
#include "minko/Signal.hpp"

namespace std
{
    template<>
    struct hash< std::pair< std::shared_ptr<minko::component::Surface>, std::shared_ptr<minko::scene::Node> > >
    {
        inline
        size_t
        operator()(const std::pair< std::shared_ptr<minko::component::Surface>, std::shared_ptr<minko::scene::Node> >& x) const
        {
            size_t seed = std::hash<long>()(long(x.first.get()));

            hash_combine(seed, std::hash<long>()(long(x.second.get())));

            return seed;
        }
    };
}

namespace minko
{
    namespace render
    {

        class DrawCallPool :
            public std::enable_shared_from_this<DrawCallPool>
        {
        public:
            typedef std::shared_ptr<DrawCallPool>                                                       Ptr;

        private:
            typedef std::shared_ptr<DrawCall>                                                           DrawCallPtr;
            typedef std::shared_ptr<math::Vector3>                                                      Vector3Ptr;
            typedef std::shared_ptr<data::Container>                                                    ContainerPtr;
            typedef std::shared_ptr<component::Surface>                                                 SurfacePtr;
            typedef std::shared_ptr<component::Renderer>                                                RendererPtr;
            typedef std::shared_ptr<render::Pass>                                                       PassPtr;
            typedef std::shared_ptr<scene::Node>                                                        NodePtr;
            typedef std::shared_ptr<data::ArrayProvider>                                                ArrayProviderPtr;
            typedef std::shared_ptr<data::AbstractFilter>                                               AbstractFilterPtr;

            typedef std::unordered_set<std::string>                                                     Techniques;

            typedef Signal<DrawCallPtr, ContainerPtr, const std::string&>                               DrawCallMacroChanged;
            typedef Signal<ContainerPtr, const std::string&>                                            PropertyChanged;
            typedef Signal<SurfacePtr, const std::string&, bool>                                        TechniqueChanged;
            typedef Signal<SurfacePtr, RendererPtr, bool>                                               VisibilityChanged;
            typedef Signal<DrawCallPtr>                                                                 ZSortNeeded;
            typedef Signal<ArrayProviderPtr, uint>                                                      ArrayIndexChanged;
            typedef Signal<RendererPtr, AbstractFilterPtr, data::BindingSource, SurfacePtr>             RendererFilterChanged;

        private:
            static const unsigned int                                                                   NUM_FALLBACK_ATTEMPTS;
            static std::unordered_map<std::string, std::pair<std::string, int>>                         _variablePropertyNameToPosition;
            static std::unordered_map<DrawCallPtr, Vector3Ptr>                                          _cachedDrawcallPositions; // in eye space

            RendererPtr                                                                                 _renderer;

            // surface that will generate new draw call next frame
            std::set<SurfacePtr>                                                                        _toCollect;
            std::set<SurfacePtr>                                                                        _toRemove;
            std::set<SurfacePtr>                                                                        _invisibleSurfaces;

            std::unordered_map<SurfacePtr, std::list<DrawCallPtr>>                                      _surfaceToDrawCalls;
            std::unordered_map<DrawCallPtr, SurfacePtr>                                                 _drawcallToSurface;
            std::unordered_map<DrawCallPtr, DrawCallMacroChanged::Slot>                                 _drawcallToMacroChangedSlot;
            std::unordered_map<DrawCallPtr, ZSortNeeded::Slot>                                          _drawcallToZSortNeededSlot;
            std::unordered_map<SurfacePtr, ContainerPtr>                                                _surfaceToRootContainer;
            std::unordered_map<SurfacePtr, uint>                                                        _surfaceToMaterialProviderIndex;
            std::list<DrawCallPtr>                                                                      _drawCalls;

            std::set<DrawCallPtr>                                                                       _dirtyDrawCalls;
            bool                                                                                        _mustZSort; // forces z-sorting at next frame

            std::unordered_map<SurfacePtr, TechniqueChanged::Slot>                                      _surfaceToTechniqueChangedSlot;
            std::unordered_multimap<SurfacePtr, VisibilityChanged::Slot>                                _surfaceToVisibilityChangedSlots;
            std::unordered_multimap<SurfacePtr, ArrayIndexChanged::Slot>                                _surfaceToIndexChangedSlots;

            std::unordered_map<SurfacePtr, std::unordered_map<std::string, Techniques>>                 _surfaceBadMacroToTechniques;
            std::unordered_map<SurfacePtr, std::unordered_map<std::string, PropertyChanged::Slot>>      _surfaceBadMacroToChangedSlot;

            RendererFilterChanged::Slot                                                                 _rendererFilterChangedSlot;

        public:
            inline static
            Ptr
            create(RendererPtr renderer)
            {
                Ptr ptr = std::shared_ptr<DrawCallPool>(new DrawCallPool(renderer));

                ptr->initialize();

                return ptr;
            }

            const std::list<std::shared_ptr<DrawCall>>&
            drawCalls();

            void
            addSurface(SurfacePtr);

            void
            removeSurface(SurfacePtr);

        private:
            explicit
            DrawCallPool(RendererPtr renderer);

            void
            initialize();

            // generate draw call for one mesh
            std::shared_ptr<DrawCall>
            initializeDrawCall(SurfacePtr,
                               PassPtr,
                               DrawCallPtr = nullptr);

            std::shared_ptr<Program>
            getWorkingProgram(SurfacePtr,
                              PassPtr,
                              FormatNameFunction,
                              ContainerPtr fullTargetData,
                              ContainerPtr fullRendererData,
                              ContainerPtr fullRootData,
                              ContainerPtr targetData,
                              ContainerPtr rendererData,
                              ContainerPtr rootData);

            std::list<DrawCallPtr>&
            generateDrawCall(SurfacePtr, unsigned int numAttempts);

            void
            refreshDrawCall(DrawCallPtr);

            void
            deleteDrawCalls(SurfacePtr);

            void
            cleanSurface(SurfacePtr);

            void
            techniqueChangedHandler(SurfacePtr, const std::string& technique, bool updateDrawCall);

            void
            visibilityChangedHandler(SurfacePtr, RendererPtr, bool visibility);

            void
            drawCallVariablesChangedHandler(ArrayProviderPtr, uint index, SurfacePtr);

            const data::MacroBindingMap
            getDrawCallmacroBindings(DrawCallPtr drawcall);

            void
            drawcallMacroChangedHandler(DrawCallPtr, ContainerPtr, const std::string&);

            void
            drawcallZSortNeededHandler(DrawCallPtr);

            void
            surfaceBadMacroChangedHandler(SurfacePtr, const std::string&);

            void
            rendererFilterChangedHandler(RendererPtr, AbstractFilterPtr, data::BindingSource, SurfacePtr);

            std::string
            formatPropertyName(const std::string&                               rawPropertyName,
                               std::unordered_map<std::string, std::string>&    variablesToValue);

            static
            Vector3Ptr
            getDrawcallEyePosition(DrawCallPtr, Vector3Ptr output = nullptr);

            static
            bool
            compareDrawCalls(DrawCallPtr, DrawCallPtr);
        };
    }
}

