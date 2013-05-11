#pragma once

#include "minko/Common.hpp"
#include "minko/SignalConnection.hpp"

namespace minko
{
	template <typename... A>
	class Signal :
		public std::enable_shared_from_this<Signal<A...>>
	{
	public:
		typedef std::shared_ptr<Signal<A...>>			ptr;
		typedef std::shared_ptr<SignalConnection<A...>>	cd;

	private:
		typedef std::function<void(A...)>				Callback;
		typedef typename std::list<cd>::iterator		ConnectionIterator;

	private:
		std::list<cd>					_connections;

		bool							_locked;
		std::list<cd>					_toAdd;
		std::list<ConnectionIterator>	_toRemove;

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
				callback
			);

			if (_locked)
				_toAdd.push_back(connection);
			else
				_connections.push_back(connection);

			return connection;
		}

		void
		remove(cd connection)
		{
			ConnectionIterator it = std::find(
				_connections.begin(), _connections.end(), connection
			);

			if (it == _connections.end() || connection->_signal != Signal<A...>::shared_from_this())
				throw std::invalid_argument("connection");

			if (_locked)
				_toRemove.push_back(it);
			else
			{
				_connections.erase(it);
				connection->_signal = nullptr;
			}
		}

		cd
		swap(cd connection, Callback callback)
		{
			if (connection == nullptr)
				return add(callback);

			ConnectionIterator it = std::find(
				_connections.begin(), _connections.end(), connection
			);

			if (it == _connections.end() || connection->_signal != Signal<A...>::shared_from_this())
				throw std::invalid_argument("connection");

			connection->_callback = callback;

			return connection;
		}

		inline
		void
		execute(A... arguments)
		{
			_locked = true;
			for (auto connection : _connections)
				connection->_callback(arguments...);
			_locked = false;

			for (auto connection : _toAdd)
				_connections.push_back(connection);

			for (auto connectionIt : _toRemove)
			{
				(*connectionIt)->_signal = nullptr;
				_connections.erase(connectionIt);
			}
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
