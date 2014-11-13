/************************************************************************************

PublicHeader:   Kernel
Filename    :   OVR_Observer.h
Content     :   Observer pattern
Created     :   June 20, 2014
Author      :   Chris Taylor

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#ifndef OVR_Observer_h
#define OVR_Observer_h

#include "OVR_Types.h"
#include "OVR_Atomic.h"
#include "OVR_RefCount.h"
#include "OVR_Delegates.h"
#include "OVR_Array.h"
#include "OVR_String.h"
#include "OVR_Hash.h"

namespace OVR {

template<class DelegateT> class Observer;
template<class DelegateT> class ObserverScope;
template<class DelegateT> class ObserverHash;


//-----------------------------------------------------------------------------
// Observer pattern

// An Observer will observe a Subject.  The Subject can emit callbacks that get
// serviced by the Observers.

// The trickiest part of this is the shutdown code.
// To simplify shutdown, the Observer is a reference-counted object divorced
// from the handler that is called.  To avoid misuse, the ObserverScope object
// is provided to ensure that the Shutdown() method is called when it goes out
// of scope.

// The Observer<> class doubles as the subject class.
// To avoid misuse, assertions are added if a subject tries to observe, or if
// an observer tries to be watched.

/*
    Usage example:

    Say we want to invoke a handler with the signature:

        void MyHandler(int i, bool b);

    The corresponding delegate type is:

        typedef Delegate2<void, int, bool> Handler;

    Note: The return value will be ignored for the Observer pattern.

    For this example there are two classes, one that emits events and another
    that listens for events:
*/

/*
    Event emitter example:

    class MyEmitter
    {
        ObserverScope<Handler> TheSubject;

    public:
        void ClearAllListeners()
        {
            TheSubject.ReleaseAll();
        }

        void CallListeners(int x, bool y)
        {
            TheSubject->Call(x, y);
        }

        Observer<Handler>* GetSubject()
        {
            return TheSubject;
        }
    };
*/

/*
    Event listener example:

    class MyListener
    {
        ObserverScope<Handler> TheObserver;

        void OnEvent(int x, bool y)
        {
            // Handle event here
        }

    public:
        MyListener()
        {
            TheObserver.SetHandler(
                Handler::FromMember<MyListener, &MyListener::OnEvent>(this)
                );
        }

        void ClearListener()
        {
            TheObserver.ReleaseAll();
        }

        void ListenTo(Observer<Handler>* emitter)
        {
            TheObserver->Observe(emitter);
        }
    };
*/

/*
    Usage example:

    MyListener listener;
    MyEmitter emitter;

    // To listen to an emitter,
    listener.ListenTo(emitter.GetSubject());

    // To call the listeners,
    emitter.CallListeners(22, true);
*/

template<class DelegateT>
class Observer : public RefCountBase< Observer<DelegateT> >
{
	friend class ObserverScope<DelegateT>;
	friend class ObserverHash<DelegateT>;

public:
    typedef Observer<DelegateT> ThisType;
    typedef DelegateT Handler;

protected:
	bool                     IsShutdown; // Flag to indicate that the object went out of scope
	mutable Lock             TheLock;    // Lock to synchronize calls and shutdown
	Array< Ptr< ThisType > > References; // List of observed or observing objects
	Handler                  TheHandler; // Observer-only: Handler for callbacks

	Observer() :
		IsShutdown(false)
	{
		TheHandler.Invalidate();
	}
	Observer(Handler handler) :
		IsShutdown(false),
		TheHandler(handler)
	{
	}
	~Observer()
	{
		OVR_ASSERT(References.GetSizeI() == 0);
	}

public:
	void SetHandler(Handler handler)
	{
		OVR_ASSERT(References.GetSizeI() == 0);
		TheHandler = handler;
	}

	// Release references and prevent further actions
	void Shutdown()
	{
		Lock::Locker locker(&TheLock);
		IsShutdown = true;
		References.ClearAndRelease();
	}

	// Get count of references held
	int GetSizeI() const
	{
		Lock::Locker locker(&TheLock);
		return References.GetSizeI();
	}

	// Observe a subject
	bool Observe(ThisType *subject)
	{
		OVR_ASSERT(TheHandler.IsValid());

		if (!subject)
		{
			return false;
		}

		Lock::Locker locker(&TheLock);

		if (IsShutdown)
		{
			return false;
		}

		if (!subject->SubjectAddObserver(this))
		{
			return false;
		}

		References.PushBack(subject);
		return true;
	}

protected:
	// Subject function: AddObserver()
	// Returns true if the observer was added
	bool SubjectAddObserver(ThisType* observer)
	{
		OVR_ASSERT(!TheHandler.IsValid());

		if (!observer)
		{
			return true;
		}

		Lock::Locker locker(&TheLock);

		if (IsShutdown)
		{
			return false;
		}

		const int count = References.GetSizeI();
		for (int i = 0; i < count; ++i)
		{
			if (References[i] == observer)
			{
				// Already watched
				return true;
			}
		}

		References.PushBack(observer);

		return true;
	}

public:
    // Subject function: Call()
#define OVR_OBSERVER_CALL_BODY(params) \
    bool callSuccess = false; \
	Lock::Locker locker(&TheLock); \
	int count = References.GetSizeI(); \
	for (int i = 0; i < count; ++i) \
	{ \
		if (!References[i]->IsShutdown) \
		{ \
			OVR_ASSERT(References[i]->TheHandler.IsValid()); \
			References[i]->TheHandler params; \
            callSuccess = true; \
		} \
		if (References[i]->IsShutdown) \
		{ \
			References.RemoveAt(i); \
			--i; --count; \
		} \
	} \
    return callSuccess;

	// Call: Various parameter counts
    // Returns true if a call was made
	bool Call()
	{
		OVR_OBSERVER_CALL_BODY(());
	}
	template<class Param1>
    bool Call(Param1& p1)
	{
		OVR_OBSERVER_CALL_BODY((p1));
	}
	template<class Param1>
    bool Call(Param1* p1)
	{
		OVR_OBSERVER_CALL_BODY((p1));
	}
	template<class Param1, class Param2>
    bool Call(Param1& p1, Param2& p2)
	{
		OVR_OBSERVER_CALL_BODY((p1, p2));
	}
	template<class Param1, class Param2>
    bool Call(Param1* p1, Param2* p2)
	{
		OVR_OBSERVER_CALL_BODY((p1, p2));
	}
	template<class Param1, class Param2, class Param3>
    bool Call(Param1& p1, Param2& p2, Param3& p3)
	{
		OVR_OBSERVER_CALL_BODY((p1, p2, p3));
	}
	template<class Param1, class Param2, class Param3>
    bool Call(Param1* p1, Param2* p2, Param3* p3)
	{
		OVR_OBSERVER_CALL_BODY((p1, p2, p3));
	}

#undef OVR_OBSERVER_CALL_BODY
};


//-----------------------------------------------------------------------------
// ObserverScope

// Scoped shutdown of the Observer object
template<class DelegateT>
class ObserverScope : public NewOverrideBase
{
	Ptr< Observer<DelegateT> > TheObserver;
	DelegateT TheHandler;

	void Shutdown()
	{
		if (TheObserver)
		{
			TheObserver->Shutdown();
			TheObserver.Clear();
		}
	}

public:
	ObserverScope()
	{
		TheObserver = *new Observer<DelegateT>;
	}
	~ObserverScope()
	{
		Shutdown();
	}

	// Release all references and recreate it
	void ReleaseAll()
	{
		Shutdown();
		TheObserver = *new Observer<DelegateT>;
		if (TheHandler.IsValid())
		{
			TheObserver->SetHandler(TheHandler);
		}
	}

	void SetHandler(DelegateT handler)
	{
		TheHandler = handler;
		TheObserver->SetHandler(handler);
	}

	Observer<DelegateT>* GetPtr()
	{
		return TheObserver.GetPtr();
	}
	Observer<DelegateT>* operator->()
	{
		return TheObserver.GetPtr();
	}
	const Observer<DelegateT>* operator->() const
	{
		return TheObserver.GetPtr();
	}
	operator Observer<DelegateT>*()
	{
		return TheObserver.GetPtr();
	}
};


//-----------------------------------------------------------------------------
// ObserverHash

// A hash containing Observers
template<class DelegateT>
class ObserverHash : public NewOverrideBase
{
public:
	ObserverHash() {}
	~ObserverHash() {Clear();}
	void Clear()
	{
		Lock::Locker locker(&TheLock);
		typename OVR::Hash< String, Ptr<Observer<DelegateT> >, OVR::String::HashFunctor >::Iterator it = _Hash.Begin();
		for( it = _Hash.Begin(); it != _Hash.End(); ++it )
		{
			Ptr<Observer<DelegateT> > o = it->Second;
			o->Shutdown();
		}
	}

	Ptr<Observer<DelegateT> > GetSubject(OVR::String key)
	{
		Lock::Locker locker(&TheLock);
		Ptr<Observer<DelegateT> > *o = _Hash.Get(key);
		if (o)
			return (*o);
		return NULL;
	}

	// Add handler to new observer with implicit creation of subject.
	void AddObserverToSubject(OVR::String key, Observer<DelegateT> *observer)
	{
		Lock::Locker locker(&TheLock);
		Ptr<Observer<DelegateT> > *subjectPtr = _Hash.Get(key);

		if (subjectPtr==NULL)
		{
			Ptr<Observer<DelegateT> > subject = *new Observer<DelegateT>();
			_Hash.Add(key, subject);
			observer->Observe(subject);
		}
		else
		{
			observer->Observe(*subjectPtr);
		}
	}

	void RemoveSubject(OVR::String key)
	{
		Lock::Locker locker(&TheLock);
		Ptr<Observer<DelegateT> > *subjectPtr = _Hash.Get(key);
		if (subjectPtr!=NULL)
		{
			(*subjectPtr)->Shutdown();
			_Hash.Remove(key);
		}
	}

protected:
	OVR::Hash< OVR::String, Ptr<Observer<DelegateT> >, OVR::String::HashFunctor > _Hash;
	Lock                     TheLock;      // Lock to synchronize calls and shutdown
};


} // namespace OVR

#endif // OVR_Observer_h
