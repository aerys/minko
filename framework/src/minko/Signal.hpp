#pragma once

#include "minko/Common.hpp"

namespace minko
{
	template <typename... A>
	class Signal :
		public std::enable_shared_from_this<Signal<A...>>
	{
	private:
		typedef std::function<void(A...)>				Callback;
		typedef typename std::list<Callback>::iterator	CallbackIterator;

		template <typename... A>
		class SignalSlot;

	public:
		typedef std::shared_ptr<Signal<A...>>			ptr;
		typedef std::shared_ptr<SignalSlot<A...>>		Slot;

	private:
		std::list<Callback>								_callbacks;
		std::list<unsigned int>							_connectionIds;
		unsigned int 									_nextConnectId;

		bool											_locked;
		std::list<std::pair<Callback, unsigned int>>	_toAdd;
		std::list<CallbackIterator>						_toRemove;

	private:
		Signal() :
			std::enable_shared_from_this<Signal<A...>>(),
			_locked(false)
		{
		}

		void
		removeConnectionById(const unsigned int connectionId)
		{
			auto connectionIdIt = _connectionIds.begin();
			auto callbackIt 	= _callbacks.begin();
			auto id 			= 0;

			while (connectionIdIt != _connectionIds.end() && *connectionIdIt != connectionId)
			{
				connectionIdIt++;
				callbackIt++;
			}

			removeConnectionByIterator(connectionIdIt, callbackIt);
		}

		void
		removeConnectionByIterator(std::list<unsigned int>::iterator connectionIdIt,
								   CallbackIterator				 	 callbackIt)
		{
			_connectionIds.erase(connectionIdIt);

			if (_locked)
				_toRemove.push_back(callbackIt);
			else
				_callbacks.erase(callbackIt);
		}

	public:
		static
		ptr
		create()
		{
			return std::shared_ptr<Signal<A...>>(new Signal<A...>());
		}

		Slot
		connect(Callback callback)
		{
			auto connection = SignalSlot<A...>::create(Signal<A...>::shared_from_this(), _nextConnectId++);

			if (_locked)
				_toAdd.push_back(std::pair<Callback, unsigned int>(callback, connection->_id));
			else
			{
				_callbacks.push_back(callback);
				_connectionIds.push_back(connection->_id);
			}

			return connection;
		}

		inline
		void
		execute(A... arguments)
		{
			_locked = true;
			for (auto callback : _callbacks)
				callback(arguments...);
			_locked = false;

			for (auto callbackAndConnectionId : _toAdd)
			{
				_callbacks.push_back(callbackAndConnectionId.first);
				_connectionIds.push_back(callbackAndConnectionId.second);
			}
			_toAdd.clear();

			for (auto callbackIt : _toRemove)
				_callbacks.erase(callbackIt);
			_toRemove.clear();
		}

	private:

		template <typename... T>
		class SignalSlot :
			public std::enable_shared_from_this<SignalSlot<T...>>
		{
			friend class Signal<T...>;

		public:
			typedef std::shared_ptr<SignalSlot<T...>>	ptr;

		public:
			std::shared_ptr<Signal<T...>>
			signal()
			{
				return _signal;
			}

			~SignalSlot()
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
				return std::shared_ptr<SignalSlot<T...>>(new SignalSlot(signal, id));
			}

			SignalSlot(std::shared_ptr<Signal<T...>> signal, const unsigned int id) :
				_signal(signal),
				_id(id)
			{
			}
		};

	};
}
