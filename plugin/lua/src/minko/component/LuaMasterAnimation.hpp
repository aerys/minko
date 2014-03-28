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

#include "minko/component/MasterAnimation.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/math/Vector3.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace component
	{
		class LuaMasterAnimation :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<MasterAnimation>("MasterAnimation")
					.property("loopStartTime",  static_cast<uint (MasterAnimation::*)() const>(&MasterAnimation::loopStartTime))
					.property("loopEndTime",	static_cast<uint (MasterAnimation::*)() const>(&MasterAnimation::loopEndTime))
					.property("isPlaying",		static_cast<bool (MasterAnimation::*)() const>(&MasterAnimation::isPlaying))
					.property("isLooping",		static_cast<bool (MasterAnimation::*)() const>(&MasterAnimation::isLooping))
					.method("hasLabel",			static_cast<bool (MasterAnimation::*)(const std::string&) const>(&MasterAnimation::hasLabel))
					.method("addLabel",			static_cast<AbstractAnimation::Ptr (MasterAnimation::*)(const std::string&, uint)>(&MasterAnimation::addLabel))
					.method("play",				static_cast<AbstractAnimation::Ptr (MasterAnimation::*)()>(&MasterAnimation::play))
					.method("stop",				static_cast<AbstractAnimation::Ptr (MasterAnimation::*)()>(&MasterAnimation::stop))
					.method("currentTime",		static_cast<uint (MasterAnimation::*)() const>(&MasterAnimation::currentTime))
					.method("resetPlaybackWindow",		static_cast<AbstractAnimation::Ptr (MasterAnimation::*)()>(&MasterAnimation::resetPlaybackWindow))
					.methodWrapper("seekTime",	&LuaMasterAnimation::seekTimeWrapper)
					.methodWrapper("seekLabel",	&LuaMasterAnimation::seekLabelWrapper)
					.methodWrapper("labelTime", &LuaMasterAnimation::labelTimeWrapper)
					.methodWrapper("setPlaybackWindowLabel",	&LuaMasterAnimation::setPlaybackWindowLabelWrapper);

					// FIXME: Bind the rest of MasterAnimation.
					// AbstractAnimation::Ptr
					// play();

					// AbstractAnimation::Ptr
					// stop();

					// AbstractAnimation::Ptr
					// addLabel(const std::string& name, uint time);

					// AbstractAnimation::Ptr
					// changeLabel(const std::string& name, const std::string& newName);

					// AbstractAnimation::Ptr
					// setTimeForLabel(const std::string& name, uint newTime);

					// AbstractAnimation::Ptr
					// removeLabel(const std::string& name);

					// AbstractAnimation::Ptr
					// setPlaybackWindow(uint, uint, bool forceRestart = false);

					// AbstractAnimation::Ptr
					// setPlaybackWindow(const std::string&, const std::string&, bool forceRestart = false);

					// AbstractAnimation::Ptr
					// resetPlaybackWindow();
			}

			static
			void
			seekTimeWrapper(MasterAnimation::Ptr animation, uint time)
			{
				animation->seek(time);
			}

			static
			void
			seekLabelWrapper(MasterAnimation::Ptr animation, std::string label)
			{
				animation->seek(label);
			}

			static
			int
			labelTimeWrapper(MasterAnimation::Ptr animation, std::string label)
			{
				return animation->labelTime(label);
			}

			static
			void
			setPlaybackWindowLabelWrapper(MasterAnimation::Ptr animation, std::string& begin, std::string& end)
			{
				animation->setPlaybackWindow(begin, end);
			}
		};
	}
}
