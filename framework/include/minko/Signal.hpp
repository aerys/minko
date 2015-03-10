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
	class Signal
	{
	public:
		typedef std::function<void(A...)>	    Callback;
		typedef std::shared_ptr<Signal<A...>>   Ptr;

	private:
		template <typename... B>
		class SignalSlot;

        typedef std::weak_ptr<SignalSlot<A...>>             SlotWeakPtr;
        typedef std::tuple<float, Callback, SlotWeakPtr>	CallbackRecord;
        typedef std::list<CallbackRecord>                   CallbackCollection;
		typedef typename CallbackCollection::iterator		CallbackIterator;

    public:
		typedef std::shared_ptr<SignalSlot<A...>>   Slot;

	private:
        CallbackCollection  _callbacks;

	public:
        Signal()
        {
        }

		Signal(const Signal& other)
		{
		}

        Signal(Signal&& other)
        {
            std::move(other._callbacks.begin(), other._callbacks.end(), _callbacks.begin());
            assert(other._callbacks.size() == 0);
        }

        Signal&
        operator=(Signal&& other)
        {
            std::move(other._callbacks.begin(), other._callbacks.end(), _callbacks.begin());
            assert(other._callbacks.size() == 0);

            return *this;
        }

        ~Signal()
        {
            for (const auto& callback : _callbacks)
            {
                auto slot = std::get<2>(callback).lock();

                if (slot)
                    slot->_signal = nullptr;
            }
            _callbacks.clear();
        }

        inline
		static
		Ptr
		create()
		{
			return std::make_shared<Signal<A...>>();
		}

		inline
		uint
		numCallbacks() const
		{
			return _callbacks.size();
		}

		Slot
		connect(Callback callback, float priority = 0)
		{
			auto connection = std::make_shared<SignalSlot<A...>>(this);

            _callbacks.push_back(CallbackRecord(priority, callback, connection));
            connection->_it = std::prev(_callbacks.end());

			if (_callbacks.size() >= 2)
			{
				auto prec = std::prev(_callbacks.end(), 2);

                if (priority > std::get<0>(*prec))
                {
                    _callbacks.sort([](const CallbackRecord& callback1, const CallbackRecord& callback2)
                    {
                        return std::get<0>(callback1) > std::get<0>(callback2);
                    });
                }
			}

			return connection;
		}

		void
		execute(A... arguments) const
		{
            auto callbacks = _callbacks;
            for (auto& callback : callbacks)
                if (!std::get<2>(callback).expired())
				    std::get<1>(callback)(arguments...);

#ifdef DEBUG
            for (auto& callback : _callbacks)
                assert(!std::get<2>(callback).expired());
#endif // DEBUG
		}

        inline
        Slot
        operator+=(Callback callback)
        {
            return connect(callback);
        }

	private:
		template <typename... T>
		class SignalSlot :
			public std::enable_shared_from_this<SignalSlot<T...>>
		{
			friend class Signal<T...>;

		public:
			typedef std::shared_ptr<SignalSlot<T...>>	Ptr;

		private:
			Signal<T...>* 	  _signal;
            CallbackIterator  _it;

		public:
			SignalSlot(Signal<T...>* signal) :
				_signal(signal)
			{
			}

			const Signal<T...>*
			signal()
			{
				return _signal;
			}

			void
			disconnect()
			{
                if (_signal)
                {
                    _signal->_callbacks.erase(_it);
                    _signal = nullptr;
                }
			}

			~SignalSlot()
			{
				disconnect();
			}

		};

	};
}
