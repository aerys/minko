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

#include "minko/component/Animation.hpp"
#include "minko/math/Matrix4x4.hpp"
#include "minko/math/Vector3.hpp"

#include "minko/LuaWrapper.hpp"

namespace minko
{
	namespace component
	{
		class LuaAnimation :
			public LuaWrapper
		{
		public:
			static
			void
			bind(LuaGlue& state)
			{
				state.Class<Animation>("Animation")
					.property("loopStartTime",  static_cast<uint (Animation::*)() const>(&Animation::loopStartTime))
					.property("loopEndTime",	static_cast<uint (Animation::*)() const>(&Animation::loopEndTime))
					.property("isPlaying",		static_cast<bool (Animation::*)() const>(&Animation::isPlaying))
					.property("isLooping",		static_cast<bool (Animation::*)() const>(&Animation::isLooping))
					.property("maxTime",		static_cast<uint(Animation::*)() const>(&Animation::maxTime))
					.method("hasLabel",			static_cast<bool (Animation::*)(const std::string&) const>(&Animation::hasLabel))
					.method("play",				static_cast<AbstractAnimation::Ptr (Animation::*)()>(&Animation::play))
					.method("stop",				static_cast<AbstractAnimation::Ptr (Animation::*)()>(&Animation::stop))
					.method("currentTime",		static_cast<uint (Animation::*)() const>(&Animation::currentTime))
					.methodWrapper("seekTime",	&LuaAnimation::seekTimeWrapper)
					.methodWrapper("seekLabel",	&LuaAnimation::seekLabelWrapper);
			}

			static
			void
			seekTimeWrapper(Animation::Ptr animation, uint time)
			{
				animation->seek(time);
			}

			static
			void
			seekLabelWrapper(Animation::Ptr animation, std::string label)
			{
				animation->seek(label);
			}
		};
	}
}
