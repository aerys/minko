#ifndef LUAGLUE_OBJECT_BASE_H_GUARD
#define LUAGLUE_OBJECT_BASE_H_GUARD

#include <atomic>
#include <exception>

#include "LuaGlue/LuaGlueDebug.h"

template<class _Class>
class LuaGlueClass;

class LuaGlueObjectImplBase
{
	public:
		virtual ~LuaGlueObjectImplBase() { }
		virtual int put() = 0;
		
		virtual void *vget() = 0;
		virtual void *vptr() = 0;
		
		template<typename T>
		T *get() { return (T *)vget(); }
		
		template<typename T>
		T *ptr() { return (T *)vptr(); }
};

class LuaGlueObjectBase
{
	public:
		virtual ~LuaGlueObjectBase() { }
		
		virtual void put() = 0;
		virtual LuaGlueObjectImplBase *impl() const = 0;
};


template<class _Class>
class LuaGlueObjectImpl : public virtual LuaGlueObjectImplBase
{
	public:
		typedef _Class Type;
		LuaGlueObjectImpl(Type *ptr, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(ptr), owner(owner)
		{
			//LG_Debug("ctor");
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(owner)
			{
				//LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			owner = false;
		}
		
		void *vget()
		{
			_ref_cnt++;
			//LG_Debug("inc: %i", _ref_cnt.load());
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			//LG_Debug("dec: %i", _ref_cnt.load());
			return _ref_cnt;
		}
		
		//Type &ref() { return *_ptr; }
		void *vptr() { return _ptr; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<Type> *_clss;
		Type *_ptr;
		bool owner;
};

template<class _Class>
class LuaGlueObjectImpl<std::shared_ptr<_Class>> : public virtual LuaGlueObjectImplBase
{
	public:
		typedef std::shared_ptr<_Class> Type;
		typedef _Class ClassType;
		LuaGlueObjectImpl(Type *ptr, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(ptr), owner(owner)
		{
			//LG_Debug("ctor");
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(owner)
			{
				//LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			owner = false;
		}
		
		void *vget()
		{
			_ref_cnt++;
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			return _ref_cnt;
		}
		
		//Type &ref() { return *_ptr; }
		void *vptr() { return _ptr; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<ClassType> *_clss;
		Type *_ptr;
		bool owner;
};

template<class _Class>
class LuaGlueObject : public virtual LuaGlueObjectBase
{
	public:
		typedef _Class Type;
		
		LuaGlueObject() : p(0)
		{
			//LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *ptr, LuaGlueClass<Type> *clss = nullptr, bool owner = false) : p(new LuaGlueObjectImpl<Type>(ptr, clss, owner))
		{
			//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : p(rhs->impl())
		{
			//LG_Debug("ctor(LuaGlueObjectBase(%p))", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : p(rhs.p)
		{
			//LG_Debug("copy ctor(%p)", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			//LG_Debug("assign(%p)", rhs.p->vptr());
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueObject()
		{
			//if(!p)
			//	LG_Debug("p == 0");
			
			if(p && !p->put())
			{
				//LG_Debug("dtor ref count hit 0, delete impl");
				delete p;
			}
		}
		
		void put()
		{
			if(!p)
				throw new std::runtime_error("blah");
			
			if(!p->put())
			{
				//LG_Debug("put ref count hit 0, delete impl");
				delete p;
			}
		}
		
		Type &operator*() { return *p->ptr<Type>(); }
		Type *operator->() { return p->ptr<Type>(); }
		
		Type *ptr() { return p->ptr<Type>(); }
		
		LuaGlueObjectImplBase *impl() const { return p; }
	private:
		LuaGlueObjectImplBase *p;
};

template<class _Class>
class LuaGlueObject<std::shared_ptr<_Class>> : public virtual LuaGlueObjectBase
{
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> Type;
		
		LuaGlueObject() : p(0)
		{
			//LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *ptr, LuaGlueClass<ClassType> *clss = nullptr, bool owner = false) : p(new LuaGlueObjectImpl<Type>(ptr, clss, owner))
		{
			//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : p(rhs->impl())
		{
			//LG_Debug("ctor(LuaGlueObjectBase(%p))", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : p(rhs.p)
		{
			//LG_Debug("copy ctor(%p)", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			//LG_Debug("assign(%p)", rhs.p->vptr());
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueObject()
		{
			//if(!p)
			//	LG_Debug("p == 0");
			
			if(p && !p->put())
			{
				//LG_Debug("dtor ref count hit 0, delete impl");
				delete p;
				p = nullptr;
			}
			
			p = nullptr;
		}
		
		void put()
		{
			if(!p)
				throw new std::runtime_error("blah");
			
			if(!p->put())
			{
				//LG_Debug("put ref count hit 0, delete impl");
				delete p;
				p = nullptr;
			}
		}
		
		Type &operator*() { return *p->ptr<Type>(); }
		Type *operator->() { return p->ptr<Type>(); }
		
		Type *ptr() { return p->ptr<Type>(); }
		
		LuaGlueObjectImplBase *impl() const { return p; }
	private:
		LuaGlueObjectImplBase *p;
};

#endif /* LUAGLUE_OBJECT_BASE_H_GUARD */
