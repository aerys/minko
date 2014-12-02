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

#include "minko/component/AbstractAnimation.hpp"

namespace minko
{
	namespace component
	{
		class MasterAnimation :
			public AbstractAnimation
		{
		public:
			typedef std::shared_ptr<MasterAnimation>	Ptr;

		private:
			typedef std::shared_ptr<Animation>			AnimationPtr;

		private:
			std::vector<AnimationPtr>					_animations;

		public:
			AbstractAnimation::Ptr
			play();

			AbstractAnimation::Ptr
			stop();

			AbstractAnimation::Ptr
			addLabel(const std::string& name, uint time);

			AbstractAnimation::Ptr
			changeLabel(const std::string& name, const std::string& newName);

			AbstractAnimation::Ptr
			setTimeForLabel(const std::string& name, uint newTime);

			AbstractAnimation::Ptr
			removeLabel(const std::string& name);

			AbstractAnimation::Ptr
			setPlaybackWindow(uint, uint, bool forceRestart = false);

			AbstractAnimation::Ptr
			setPlaybackWindow(const std::string&, const std::string&, bool forceRestart = false);

			AbstractAnimation::Ptr
			resetPlaybackWindow();

		protected:
			MasterAnimation(const std::vector<AnimationPtr>&, bool isLooping);

			void
			update();
		};
	}
}
