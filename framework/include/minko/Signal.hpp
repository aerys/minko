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
		typedef std::function<void(A...)>										CallbackFunction;
		typedef std::pair<float, CallbackFunction>								Callback;
		typedef typename std::vector<Callback>::iterator						CallbackIterator;
		typedef typename std::vector<std::pair<float, unsigned int>>::iterator	SlotIterator;

		template <typename... B>
		class SignalSlot;

	public:
		typedef std::shared_ptr<Signal<A...>>			Ptr;
		typedef std::shared_ptr<SignalSlot<A...>>		Slot;

	private:
		std::vector<Callback>									_callbacks;
		std::vector<std::pair<float, unsigned int>>				_slotIds;
		unsigned int 											_nextSlotId;

		bool													_locked;
		std::vector<std::pair<Callback, uint>>					_toAdd;
		std::list<std::pair<CallbackIterator, SlotIterator>>	_toRemove;

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
			auto i 				= 0;

			while ((*connectionIdIt).second != connectionId)
			{
				connectionIdIt++;
				callbackIt++;
			}

			removeConnectionByIterator(connectionIdIt, callbackIt);
		}

		void
		removeConnectionByIterator(SlotIterator		 connectionIdIt,
								   CallbackIterator	 callbackIt)
		{
			if (_locked)
			{
				auto addIt = std::find_if(_toAdd.begin(), _toAdd.end(), [&](std::pair<Callback, unsigned int>& add)
				{
					return add.second == (*connectionIdIt).second;
				});

				if (addIt != _toAdd.end())
					_toAdd.erase(addIt);
				else
					_toRemove.push_back(std::pair<CallbackIterator, SlotIterator>(callbackIt, connectionIdIt));
			}
			else
			{
				_callbacks.erase(callbackIt);
				_slotIds.erase(connectionIdIt);
			}
		}

	public:
		static
		Ptr
		create()
		{
			return std::shared_ptr<Signal<A...>>(new Signal<A...>());
		}

		inline
		uint
		numCallbacks() const
		{
			return _callbacks.size();
		}

		void
		sortSignals()
		{
			std::sort(_callbacks.begin(), _callbacks.end(), [&](Callback callback1, Callback callback2) -> bool
			{
				return callback1.first > callback2.first;
			});

			std::sort(_slotIds.begin(), _slotIds.end(), [&](std::pair<float, unsigned int> slot1, std::pair<float, unsigned int> slot2) -> bool
			{
				return slot1.first > slot2.first;
			});
		}

		Slot
		connect(CallbackFunction callback, float priority = 0)
		{
			auto connection = SignalSlot<A...>::create(Signal<A...>::shared_from_this(), _nextSlotId++);
			
			if (_locked)
				_toAdd.push_back(std::pair<Callback, unsigned int>(std::pair<float, CallbackFunction>(priority, callback), connection->_id));
			else
			{
				_callbacks.push_back(std::pair<float, CallbackFunction>(priority, callback));
				_slotIds.push_back(std::pair<float, unsigned int>(priority, connection->_id));
				sortSignals();
			}

			return connection;
		}

		void
		execute(A... arguments)
		{
			_locked = true;
			for (auto& callback : _callbacks)
				std::get<1>(callback)(arguments...);
			_locked = false;

			for (auto& callbackAndConnectionId : _toAdd)
			{
				_callbacks.push_back(callbackAndConnectionId.first);
				_slotIds.push_back(std::pair<float, unsigned int>(callbackAndConnectionId.first.first, callbackAndConnectionId.second));
				sortSignals();
			}
			
			for (auto& callbackIt : _toRemove)
			{
				_callbacks.erase(callbackIt.first);
				_slotIds.erase(callbackIt.second);
			}

			_toAdd.clear();
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

			void
			disconnect()
			{
				if (_signal != nullptr)
				{
					_signal->removeConnectionById(_id);
					_signal = nullptr;
				}				
			}

			~SignalSlot()
			{
				disconnect();
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
