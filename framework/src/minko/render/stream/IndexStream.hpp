#pragma once

#include "minko/Common.hpp"

namespace
{
	using namespace minko::render::context;
}

namespace minko
{
	namespace render
	{
		namespace stream
		{
			class IndexStream
			{
			public:
				typedef std::shared_ptr<IndexStream>	Ptr;

			private:
				std::vector<unsigned short>			_data;
				std::shared_ptr<AbstractContext>	_context;
				int									_buffer;

			public:
				inline static
				Ptr
				create(std::shared_ptr<AbstractContext> context, std::vector<unsigned short>& data)
				{
					return std::shared_ptr<IndexStream>(new IndexStream(context, data));
				}

				inline static
				Ptr
				create(std::shared_ptr<AbstractContext> context, unsigned short* begin, unsigned short* end)
				{
					return std::shared_ptr<IndexStream>(new IndexStream(context, begin, end));
				}

				inline
				const std::vector<unsigned short>
				data()
				{
					return _data;
				}

				inline
				const int
				buffer()
				{
					return _buffer;
				}

			private:
				IndexStream(std::shared_ptr<AbstractContext>	context,
							std::vector<unsigned short>			data) :
					_data(data),
					_context(context),
					_buffer(-1)
				{
					upload();
				}

				IndexStream(std::shared_ptr<AbstractContext>	context,
							unsigned short*						begin,
							unsigned short*						end) :
					_data(begin, end),
					_context(context),
					_buffer(-1)
				{
					upload();
				}

				void
				upload();
			};
		}
	}
}
