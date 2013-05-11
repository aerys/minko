#pragma once

#include "minko/Common.hpp"

namespace minko
{
	template <typename... T>
	class SignalConnection
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

	private:
		std::shared_ptr<Signal<T...>>	_signal;
		std::function<void(T...)>		_callback;

	private:
		inline static
		ptr
		create(std::shared_ptr<Signal<T...>> signal, std::function<void(T...)> callback)
		{
			return std::shared_ptr<SignalConnection<T...>>(new SignalConnection(signal, callback));
		}

		SignalConnection(std::shared_ptr<Signal<T...>> signal, std::function<void(T...)> callback) :
			_signal(signal),
			_callback(callback)
		{
		}
	};
}