#pragma once

#include "minko/Common.hpp"

namespace minko
{
	template <typename... A>
	class Signal
	{
	public:
		typedef std::shared_ptr<Signal<A...>>	ptr;
		typedef std::shared_ptr<int>			cd;

	private:
		typedef std::function<void(A...)>	Callback;

		std::list<Callback>	_callbacks;

	private:
		Signal()
		{}

	public:
		static
		ptr
		create()
		{
			return std::shared_ptr<Signal<A...>>(new Signal<A...>());
		}

		cd
		add(Callback callback)
		{
			_callbacks.push_back(callback);

			return std::make_shared<int>(_callbacks.size() - 1);
		}

		void
		remove(cd callbackId)
		{
			int index = *callbackId;

			// invalid callback descriptor
			if (index < 0)
				throw;

			typename std::list<Callback>::iterator it = _callbacks.begin();

			std::advance(it, index);
			_callbacks.erase(it);

			*callbackId = -1;
		}

		inline
		void
		execute(A... arguments)
		{
			for (auto callback : _callbacks)
				callback(arguments...);
		}

		inline
		cd
		operator+=(Callback callback)
		{
			return add(callback);
		}

		inline
		void
		operator-=(cd callbackId)
		{
			remove(callbackId);
		}

		inline
		void
		operator()(A... arguments)
		{
			return execute(arguments...);
		}
	};
}
