#pragma once

#include "minko/Common.hpp"
#include "minko/Signal.hpp"

namespace
{
}

namespace minko
{
	namespace scene
	{
		namespace data
		{
			class Value
			{
			public:
				typedef std::shared_ptr<Value> ptr;

			protected:
				std::shared_ptr<Signal<ptr>>	_changed;

			public:
				inline
				std::shared_ptr<Signal<ptr>>
				changed()
				{
					return _changed;
				}

				virtual
				~Value()
				{
				}

			protected:
				Value() :
					_changed(Signal<ptr>::create())
				{
				}

				Value(std::shared_ptr<Signal<ptr>> changed) :
					_changed(changed)
				{
				}
			};
		}
	}
}
