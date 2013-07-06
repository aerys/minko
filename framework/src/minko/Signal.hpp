/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

		template <typename... B>
		class SignalSlot;

	public:
		typedef std::shared_ptr<Signal<A...>>			Ptr;
		typedef std::shared_ptr<SignalSlot<A...>>		Slot;

	private:
		std::list<Callback>								_callbacks;
		std::list<unsigned int>							_slotIds;
		unsigned int 									_nextSlotId;

		bool											_locked;
		std::list<std::pair<Callback, unsigned int>>	_toAdd;
		std::list<CallbackIterator>						_toRemove;

	private:
		Signal() :
			std::enable_shared_from_this<Signal<A...>>(),
            _nextSlotId(0),
			_locked(false)
		{
		}

		void
		removeConnectionById(const unsigned int connectionId)
		{
			auto connectionIdIt = _slotIds.begin();
			auto callbackIt 	= _callbacks.begin();
			auto id 			= 0;

			while (connectionIdIt != _slotIds.end() && *connectionIdIt != connectionId)
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
			_slotIds.erase(connectionIdIt);

			if (_locked)
				_toRemove.push_back(callbackIt);
			else
				_callbacks.erase(callbackIt);
		}

	public:
		static
		Ptr
		create()
		{
			return std::shared_ptr<Signal<A...>>(new Signal<A...>());
		}

		Slot
		connect(Callback callback)
		{
			auto connection = SignalSlot<A...>::create(
				Signal<A...>::shared_from_this(), _nextSlotId++
			);

			if (_locked)
				_toAdd.push_back(std::pair<Callback, unsigned int>(callback, connection->_id));
			else
			{
				_callbacks.push_back(callback);
				_slotIds.push_back(connection->_id);
			}

			return connection;
		}

		void
		execute(A... arguments)
		{
			_locked = true;
			for (auto& callback : _callbacks)
				callback(arguments...);
			_locked = false;

			for (auto& callbackAndConnectionId : _toAdd)
			{
				_callbacks.push_back(callbackAndConnectionId.first);
				_slotIds.push_back(callbackAndConnectionId.second);
			}
			_toAdd.clear();

			for (auto& callbackIt : _toRemove)
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
			typedef std::shared_ptr<SignalSlot<T...>>	Ptr;

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
			Ptr
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
