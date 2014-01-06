#ifndef LUAGLUE_OBJECT_BASE_H_GUARD
#define LUAGLUE_OBJECT_BASE_H_GUARD

#if !defined(EMSCRIPTEN)
# include <atomic>
#endif
#include <exception>

#include "LuaGlue/LuaGlueDebug.h"

#ifdef LUAGLUE_TYPECHECK
#	define GetLuaUdata(state, idx, name) ((LuaGlueObjectBase *)luaL_checkudata(state, idx, name))
#else
#	define GetLuaUdata(state, idx, name) ((LuaGlueObjectBase *)lua_touserdata(state, idx))
#endif

#define CastLuaGlueObject(ClassType, o) ((LuaGlueObject<ClassType> *)o)
#define CastLuaGlueObjectShared(ClassType, o) ( (LuaGlueObject<std::shared_ptr<ClassType>> *)o )

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
		LuaGlueObjectBase(bool is_shared_ptr_ = false) : is_shared_ptr(is_shared_ptr_) { }
		virtual ~LuaGlueObjectBase() { }
		
		virtual void put() = 0;
		virtual LuaGlueObjectImplBase *impl() const = 0;
		
		bool isSharedPtr() { return is_shared_ptr; }
	private:
		bool is_shared_ptr;
};


template<class _Class>
class LuaGlueObjectImpl : public virtual LuaGlueObjectImplBase
{
	public:
		typedef _Class Type;

#ifdef _MSC_VER
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _clss(clss), _ptr(p), _owner(owner)
		{
			std::atomic_init(&_ref_cnt, 1);
			//LG_Debug("ctor");
		}
#else
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(p), _owner(owner)
		{
			//LG_Debug("ctor");
		}
#endif  // _MSC_VER
				
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(_owner)
			{
				//LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			_owner = false;
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
#if defined(EMSCRIPTEN)
		int _ref_cnt;
#else
		std::atomic_int _ref_cnt;
#endif
		LuaGlueClass<Type> *_clss;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueObjectImpl<std::shared_ptr<_Class>> : public virtual LuaGlueObjectImplBase
{
	public:
		typedef std::shared_ptr<_Class> Type;
		typedef _Class ClassType;

#ifdef _MSC_VER
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _clss(clss), _ptr(p), _owner(owner)
		{
			std::atomic_init(&_ref_cnt, 1);
			//LG_Debug("ctor");
		}
#else
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(p), _owner(owner)
		{	
			//LG_Debug("ctor");
		}
#endif // _MSC_VER
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(_owner)
			{
				//LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			_owner = false;
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
#if defined(EMSCRIPTEN)
		int _ref_cnt;
#else
		std::atomic_int _ref_cnt;
#endif
		LuaGlueClass<ClassType> *_clss;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueObject : public LuaGlueObjectBase
{
	public:
		typedef _Class Type;
		
		LuaGlueObject() : LuaGlueObjectBase(false), p(0)
		{
			//LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *optr, LuaGlueClass<Type> *clss = nullptr, bool owner = false) : LuaGlueObjectBase(false), p(new LuaGlueObjectImpl<Type>(optr, clss, owner))
		{
			//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : LuaGlueObjectBase(false), p(rhs->impl())
		{
			//LG_Debug("ctor(LuaGlueObjectBase(%p))", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : LuaGlueObjectBase(false), p(rhs.p)
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
class LuaGlueObject<std::shared_ptr<_Class>> : public LuaGlueObjectBase
{
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> Type;
		
		LuaGlueObject() : LuaGlueObjectBase(true), p(0)
		{
			//LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *optr, LuaGlueClass<ClassType> *clss = nullptr, bool owner = false) : LuaGlueObjectBase(true), p(new LuaGlueObjectImpl<Type>(optr, clss, owner))
		{
			//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : LuaGlueObjectBase(true), p(rhs->impl())
		{
			//LG_Debug("ctor(LuaGlueObjectBase(%p))", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : LuaGlueObjectBase(true), p(rhs.p)
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
		
		Type operator*() { LG_Debug("operator*"); return *p->ptr<Type>(); }
		Type *operator->() { return p->ptr<Type>(); }
		
		ClassType *ptr() { return (*(p->ptr<Type>())).get(); }
		
		LuaGlueObjectImplBase *impl() const { return p; }
	private:
		LuaGlueObjectImplBase *p;
};

#endif /* LUAGLUE_OBJECT_BASE_H_GUARD */
