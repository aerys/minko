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
			typedef std::shared_ptr<Loader>	ptr;

		private:
			std::vector<char>				_data;

			std::shared_ptr<Options>		_options;

			std::shared_ptr<Signal<ptr>>	_complete;
			std::shared_ptr<Signal<ptr>>	_error;

		public:
			inline static
			ptr
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
			std::shared_ptr<Signal<ptr>>
			complete()
			{
				return _complete;
			}

			inline
			std::shared_ptr<Signal<ptr>>
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
