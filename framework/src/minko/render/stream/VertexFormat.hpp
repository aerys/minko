#pragma once

namespace minko
{
	namespace render
	{
		namespace stream
		{
			class VertexFormat
			{
			public:
				typedef std::shared_ptr<VertexFormat>	ptr;

			private:
				std::list<std::shared_ptr<VertexAttribute>> _attributes;

			public:
				inline static
				ptr
				create(const std::list<std::shared_ptr<VertexAttribute>>& attributes)
				{
					return std::shared_ptr<VertexFormat>(new VertexFormat(attributes));
				}

				inline
				const std::list<std::shared_ptr<VertexAttribute>>
				attributes()
				{
					return _attributes;
				}

			private:
				VertexFormat(const std::list<std::shared_ptr<VertexAttribute>> attributes) :
					_attributes(attributes)
				{
				}
			};
		}
	}
}