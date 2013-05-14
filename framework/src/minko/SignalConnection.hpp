#pragma once

#include "minko/Common.hpp"

namespace minko
{
	template <typename... T>
	class SignalConnection :
		public std::enable_shared_from_this<SignalConnection<T...>>
	{
		friend class Signal<T...>;

	public:
		typedef std::shared_ptr<SignalConnection<T...>>	ptr;

	public:
		std::shared_ptr<Signal<T...>>
		signal()
		{
			return _signal;
		}

		~SignalConnection()
		{
			if (_signal != nullptr)
			{
				_signal->removeConnectionById(_id);
				_signal = nullptr;
			}
		}

	private:
		std::shared_ptr<Signal<T...>>	_signal;
		const unsigned int				_id;

	private:
		inline static
		ptr
		create(std::shared_ptr<Signal<T...>> signal, const unsigned int id)
		{
			return std::shared_ptr<SignalConnection<T...>>(new SignalConnection(signal, id));
		}

		SignalConnection(std::shared_ptr<Signal<T...>> signal, const unsigned int id) :
			_signal(signal),
			_id(id)
		{
		}
	};
}