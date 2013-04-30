#pragma once

#include "minko/Common.hpp"

namespace minko
{
	namespace render
	{
		class DrawCall
		{
		public:
			typedef std::shared_ptr<DrawCall> ptr;

		private:

			unsigned int _vertexBuffer;
			unsigned int _indexBuffer;

		public:
			DrawCall();

			void
			render();
		};		
	}
}
