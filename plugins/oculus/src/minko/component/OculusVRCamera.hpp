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
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/PerspectiveCamera.hpp"

namespace minko
{
	namespace component
	{
		class OculusVRCamera :
			public AbstractComponent,
			public std::enable_shared_from_this<OculusVRCamera>
		{
		public:
			typedef std::shared_ptr<OculusVRCamera>	Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		NodePtr;
			typedef std::shared_ptr<AbstractComponent>	AbsCmpPtr;
			typedef std::shared_ptr<SceneManager>		SceneMgrPtr;
			typedef std::shared_ptr<render::Texture>	TexturePtr;

		private:
			float										_aspectRatio;

			NodePtr										_root;
			std::shared_ptr<PerspectiveCamera>			_leftCamera;
			std::shared_ptr<PerspectiveCamera>			_rightCamera;
			std::shared_ptr<Renderer>					_renderer;

			Signal<AbsCmpPtr, NodePtr>::Slot			_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot			_targetRemovedSlot;
			Signal<SceneMgrPtr, uint, TexturePtr>::Slot	_renderEndSlot;

		public:
			inline static
			Ptr
			create(float aspectRatio)
			{
				auto oc = std::shared_ptr<OculusVRCamera>(new OculusVRCamera(aspectRatio));

				oc->initialize();

				return oc;
			}

			inline
			float
			aspectRatio()
			{
				return _aspectRatio;
			}

			inline
			void
			aspectRatio(float ratio)
			{
				if (ratio != _aspectRatio)
				{
					_aspectRatio = ratio;

					_leftCamera->aspectRatio(ratio);
					_rightCamera->aspectRatio(ratio);
				}
			}

		private:
			OculusVRCamera(float aspectRatio);

			void
			initialize();

			void
			targetAddedHandler(AbsCmpPtr component, NodePtr target);

			void
			targetRemovedHandler(AbsCmpPtr component, NodePtr target);

			void
			renderEndHandler(SceneMgrPtr sceneManager, uint frameId, TexturePtr	renderTarget);
		};
	}
}

