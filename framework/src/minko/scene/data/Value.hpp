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
				typedef std::shared_ptr<Value> Ptr;

			protected:
				std::shared_ptr<Signal<Ptr>>	_changed;

			public:
				inline
				std::shared_ptr<Signal<Ptr>>
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
					_changed(Signal<Ptr>::create())
				{
				}

				Value(std::shared_ptr<Signal<Ptr>> changed) :
					_changed(changed)
				{
				}
			};
		}
	}
}
