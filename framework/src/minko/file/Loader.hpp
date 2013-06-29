#pragma once

#include "minko/Common.hpp"

namespace
{
}

namespace minko
{
	namespace file
	{
		class Loader :
			public std::enable_shared_from_this<Loader>
		{
		public:
			typedef std::shared_ptr<Loader>	Ptr;

		private:
			std::vector<char>				_data;

			std::shared_ptr<Options>		_options;

			std::shared_ptr<Signal<Ptr>>	_complete;
			std::shared_ptr<Signal<Ptr>>	_error;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Loader>(new Loader());
			}

			inline
			const std::vector<char>&
			data()
			{
				return _data;
			}

			inline
			std::shared_ptr<Options>
			options()
			{
				return _options;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			complete()
			{
				return _complete;
			}

			inline
			std::shared_ptr<Signal<Ptr>>
			error()
			{
				return _error;
			}

			void
			load(const std::string& filename, std::shared_ptr<Options> options);

		private:
			Loader();
		};
	}
}
