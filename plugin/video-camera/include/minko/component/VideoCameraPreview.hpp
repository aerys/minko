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

#include "minko/Any.hpp"
#include "minko/Common.hpp"
#include "minko/VideoCameraCommon.hpp"
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
	namespace component
	{
		class VideoCameraPreview :
			public AbstractComponent
		{
		public:
			typedef std::shared_ptr<VideoCameraPreview> Ptr;

		private:
            typedef std::shared_ptr<SceneManager> SceneManagerPtr;
			typedef std::shared_ptr<video::AbstractVideoCamera> AbstractVideoCameraPtr;

			typedef std::shared_ptr<AbstractComponent> AbstractComponentPtr;
            typedef std::shared_ptr<Surface> SurfacePtr;
            typedef std::shared_ptr<scene::Node> NodePtr;

            typedef std::shared_ptr<render::AbstractContext> AbstractContextPtr;
            typedef std::shared_ptr<render::AbstractTexture> AbstractTexturePtr;

		private:
            SceneManagerPtr _sceneManager;
            AbstractContextPtr _context;
			AbstractVideoCameraPtr _videoCamera;

            AbstractTexturePtr _videoPreviewTarget;
            SurfacePtr _previewSurface;

			Any _targetAddedSlot;
            Any _targetRemovedSlot;

            Any _frameBeginSlot;
            Any _frameEndSlot;

            Any _videoCameraFrameReceivedSlot;

		public:
			inline
			static
			Ptr
			create(SceneManagerPtr sceneManager, AbstractContextPtr context, AbstractVideoCameraPtr videoCamera)
			{
				auto instance = Ptr(new VideoCameraPreview());

                instance->_sceneManager = sceneManager;
                instance->_context = context;
				instance->_videoCamera = videoCamera;

                instance->initialize();

				return instance;
			}

            inline
            SurfacePtr 
            getPreviewSurface() 
            {
                return _previewSurface;
            }

            inline
            AbstractTexturePtr
            getVideoPreviewTarget()
            {
                return _videoPreviewTarget;
            }

		private:
			VideoCameraPreview();

			void
			initialize();

            void
            targetAddedHandler(AbstractComponentPtr component, NodePtr target);

            void
            targetRemovedHandler(AbstractComponentPtr component, NodePtr target);

            void
            frameBeginHandler(SceneManagerPtr sceneManager, float time, float deltaTime);

            void
            frameEndHandler(SceneManagerPtr sceneManager, float time, float deltaTime);

            void
            frameReceivedHandler(
                AbstractVideoCameraPtr videoCamera,
                const std::vector<unsigned char>& data,
                int width,
                int height,
                video::ImageFormatType format
            );

            void
            initializeVideoPreviewTarget(int width, int height, video::ImageFormatType format);

            void
            updateVideoPreviewTarget(const std::vector<unsigned char>& data, int width, int height, video::ImageFormatType format);
		};
	}
}
