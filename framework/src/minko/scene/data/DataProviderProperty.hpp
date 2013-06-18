#pragma once

#include "minko/Common.hpp"

namespace
{
}

namespace minko
{
	namespace scene
	{
		namespace data
		{
			template <typename T>
			class DataProviderProperty
			{
			public:
				typedef std::shared_ptr<Signal<T>> ChangedSignalPtr;

			private:
				ChangedSignalPtr _changed;

			public:
				inline
				ChangedSignalPtr
				changed()
				{
					return _changed;
				}

			protected:
				DataProviderProperty() :
					_changed(Signal<T>::create())
				{
				}
			};
		}
	}
}
