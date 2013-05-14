#pragma once

#include "minko/Common.hpp"
#include "minko/SignalConnection.hpp"

namespace minko
{
	template <typename... A>
	class Signal :
		public std::enable_shared_from_this<Signal<A...>>
	{
		friend class SignalConnection<A...>;

	public:
		typedef std::shared_ptr<Signal<A...>>			ptr;
		typedef std::shared_ptr<SignalConnection<A...>>	cd;

	private:
		typedef std::function<void(A...)>				Callback;
		typedef typename std::list<Callback>::iterator	CallbackIterator;

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
			cd connection = SignalConnection<A...>::create(
				Signal<A...>::shared_from_this(),
				_nextConnectId++
			);

			if (_locked)
				_toAdd.push_back(std::pair<Callback, unsigned int>(callback, connection->_id));
			else
			{
				_callbacks.push_back(callback);
				_connectionIds.push_back(connection->_id);
			}

			return connection;
		}

		void
		remove(cd connection)
		{
			auto connectionIdIt = _connectionIds.begin();
			auto callbackIt 	= _callbacks.begin();

			while (connectionIdIt != _connectionIds.end() && *connectionIdIt != connection->_id)
			{
				connectionIdIt++;
				callbackIt++;
			}

			if (connectionIdIt == _connectionIds.end()
				|| connection->_signal != Signal<A...>::shared_from_this())
				throw std::invalid_argument("connection");

			connection->_signal = nullptr;

			removeConnectionByIterator(connectionIdIt, callbackIt);
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

		cd
		swap(cd connection, Callback callback)
		{
			if (connection == nullptr)
				return add(callback);

			if (std::find(_connectionIds.begin(), _connectionIds.end(), connection->_id) == _connectionIds.end()
				|| connection->_signal != Signal<A...>::shared_from_this())
				throw std::invalid_argument("connection");

			connection->_callback = callback;

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
