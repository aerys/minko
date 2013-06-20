#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"
#include "minko/Any.hpp"
#include "minko/scene/data/ValueBase.hpp"

namespace
{
}

namespace minko
{
	namespace scene
	{
		namespace data
		{
			class Value :
				public ValueBase,
				public std::enable_shared_from_this<Value>
			{
			public:
				typedef std::shared_ptr<Value> ptr;

			private:
				Any	_content;

			public:
				Any&
				content()
				{
					return _content;
				}

				inline static
				ptr
				create(const Any& content)
				{
					auto v = new Value();

					v->_content = content;

					return std::shared_ptr<Value>(v);
				}

				inline static
				ptr
				create(std::shared_ptr<ValueBase> valueBase)
				{
					auto v = new Value();

					v->_content = valueBase;
					v->_changed = valueBase->changed();

					return std::shared_ptr<Value>(v);
				}
				
				inline static
				ptr
				create()
				{
					return std::shared_ptr<Value>(new Value());
				}

				template <typename ValueType>
				Any&
				operator=(const ValueType& rhs)
				{
					Any(rhs).swap(*this);

					_changed->execute(shared_from_this());

					return *this;
				}
			};
		}
	}
}
