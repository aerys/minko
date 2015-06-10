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
#include "minko/Uuid.hpp"

namespace minko
{
	namespace render
	{
		class AbstractResource
		{
		public:
			typedef std::shared_ptr<AbstractResource> Ptr;

		protected:
            const std::string                           _uuid;
			std::shared_ptr<render::AbstractContext>	_context;
			ResourceId									_id;

		public:
            inline
            const std::string&
            uuid()
            {
                return _uuid;
            }

			inline
			std::shared_ptr<render::AbstractContext>
			context()
			{
				return _context;
			}

			inline
			const ResourceId
			id()
			{
				if (_id == -1)
					throw;

				return _id;
			}

			inline
			const bool
			isReady()
			{
				return _id != -1;
			}

			virtual
			void
			dispose() = 0;

			virtual
			void
			upload() = 0;

		protected:
			AbstractResource(std::shared_ptr<render::AbstractContext> context) :
                _uuid(Uuid::getUuid()),
				_context(context),
				_id(-1)
			{
			}

			virtual
			~AbstractResource() = default;
		};
	}
}
