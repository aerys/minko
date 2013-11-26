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

#include <minko/component/AbstractComponent.hpp>
#include <minko/render/VertexBuffer.hpp>

namespace minko
{
	namespace geometry
	{
		class Skin;
	};

	namespace component
	{
		class Skinning:
			public AbstractComponent,
			public std::enable_shared_from_this<Skinning>
		{
		public:	
			typedef std::shared_ptr<Skinning>						Ptr;
	
		private:
			typedef std::shared_ptr<scene::Node>					NodePtr;
			typedef std::shared_ptr<math::Matrix4x4>				Matrix4x4Ptr;
			typedef std::shared_ptr<render::AbstractContext>		AbstractContextPtr;
			typedef std::shared_ptr<render::VertexBuffer>			VertexBufferPtr;
			typedef std::shared_ptr<component::AbstractComponent>	AbsCmpPtr;
			typedef std::shared_ptr<component::SceneManager>		SceneManagerPtr;
			typedef std::shared_ptr<geometry::Geometry>				GeometryPtr;
			typedef std::shared_ptr<geometry::Skin>					SkinPtr;
			typedef std::shared_ptr<data::Provider>					ProviderPtr;
			typedef std::shared_ptr<data::ArrayProvider>			ArrayProviderPtr;

			typedef Signal<AbsCmpPtr, NodePtr>						TargetAddedOrRemovedSignal;
			typedef Signal<NodePtr, NodePtr, NodePtr>				AddedOrRemovedSignal;
			typedef Signal<SceneManagerPtr>							SceneManagerSignal;

		public:
			static const std::string								PNAME_BONE_MATRICES;
			static const std::string								ATTRNAME_BONE_IDS_A;
			static const std::string								ATTRNAME_BONE_IDS_B;
			static const std::string								ATTRNAME_BONE_WEIGHTS_A;
			static const std::string								ATTRNAME_BONE_WEIGHTS_B;
			static const unsigned int								MAX_NUM_BONES_PER_VERTEX;

		private:
			static const std::string								ATTRNAME_POSITION;
			static const std::string								ATTRNAME_NORMAL;

		private:
			const SkinPtr											_skin;
			AbstractContextPtr										_context;

			ArrayProviderPtr										_boneMatricesData; // current frame
			render::VertexBuffer::Ptr								_boneVertexBuffer; // vertex buffer storing vertex attributes
			//ProviderPtr												_

			std::unordered_map<NodePtr, GeometryPtr>				_targetGeometry;
			std::unordered_map<NodePtr, clock_t>					_targetStartTime;

			std::unordered_map<NodePtr,	std::vector<float>>			_targetInputPositions;	// only for software skinning
			std::unordered_map<NodePtr,	std::vector<float>>			_targetInputNormals;	// only for software skinning

			TargetAddedOrRemovedSignal::Slot						_targetAddedSlot;
			TargetAddedOrRemovedSignal::Slot						_targetRemovedSlot;
			AddedOrRemovedSignal::Slot								_addedSlot;
			AddedOrRemovedSignal::Slot								_removedSlot;
			SceneManagerSignal::Slot								_frameBeginSlot;

		public:
			inline static
			Ptr
			create(const SkinPtr skin, AbstractContextPtr context)
			{
				Ptr ptr(new Skinning(skin, context));

				ptr->initialize();

				return ptr;
			}

			inline
			~Skinning()
			{
				_targetAddedSlot	= nullptr;
				_targetRemovedSlot	= nullptr;
				_addedSlot			= nullptr;
				_removedSlot		= nullptr;
				_frameBeginSlot		= nullptr;
			}

		private:
			Skinning(const SkinPtr, AbstractContextPtr);

			void
			initialize();

			void
			targetAddedHandler(AbsCmpPtr, NodePtr);

			void
			targetRemovedHandler(AbsCmpPtr, NodePtr);

			void
			addedHandler(NodePtr, NodePtr, NodePtr);

			void
			removedHandler(NodePtr, NodePtr, NodePtr);

			void
			findSceneManager();

			void
			setSceneManager(SceneManagerPtr);

			void
			frameBeginHandler(SceneManagerPtr);

			void
			updateFrame(NodePtr, unsigned int frameId);

			void
			performSoftwareSkinning(NodePtr, const std::vector<Matrix4x4Ptr>&);

			void
			performSoftwareSkinning(render::VertexBuffer::AttributePtr, 
									render::VertexBuffer::Ptr, 
									const std::vector<float>&, 
									const std::vector<Matrix4x4Ptr>&, 
									bool doDeltaTransform);

			render::VertexBuffer::Ptr
			createVertexBufferForBones() const;
		};
	}
}
