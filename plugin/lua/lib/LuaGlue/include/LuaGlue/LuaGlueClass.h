#ifndef LUAGLUE_CLASS_H_GUARD
#define LUAGLUE_CLASS_H_GUARD

#include <vector>
#include <string>
#include <map>
#include <new>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

#include "LuaGlue/LuaGlueUtils.h"
#include "LuaGlue/LuaGlueDebug.h"

#include "LuaGlue/LuaGlueBase.h"

template<typename _Class, typename... _Args>
class LuaGlueCtorMethod;

template<typename _Class>
class LuaGlueDtorMethod;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueStaticMethod;

template<typename _Class, typename... _Args>
class LuaGlueStaticMethod<void, _Class, _Args...>;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueConstMethod;

template<typename _Class, typename... _Args>
class LuaGlueConstMethod<void, _Class, _Args...>;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod;

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...>;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueIndexMethod;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueNewIndexMethod;

#include "LuaGlueProperty.h"

template<class _Class>
class LuaGlueObject;

template<typename _Class>
class LuaGlueClass : public LuaGlueClassBase
{
	public:
		static const char METATABLE_CLASSNAME_FIELD[];
		static const char METATABLE_INTCLASSNAME_FIELD[];
		static const char METATABLE_CLASSIDX_FIELD[];
		
		typedef _Class ClassType;
		
		LuaGlueClass(LuaGlueBase *luaGlue, const std::string &name) : luaGlue_(luaGlue), name_(name)
		{ }
		
		~LuaGlueClass()
		{ 	}
		
		const std::string &name() { return name_; }
		
		template<typename _Ret, typename... _Args>
		_Ret invokeMethod(const std::string &name, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			pushInstance(luaGlue_->state(), obj);
			//printf("attempting to get method %s::%s\n", name_.c_str(), name.c_str());
			lua_getfield(luaGlue_->state(), -1, name.c_str());
			//lua_dump_stack(luaGlue_->state());
		
			lua_pushvalue(luaGlue_->state(), -2);
			applyTuple(luaGlue_, luaGlue_->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*luaGlue_, luaGlue_->state(), args), void(), '\0')... };
			
			lua_call(luaGlue_->state(), Arg_Count_+1, 1);
			lua_remove(luaGlue_->state(), -2);
			return stack<_Ret>::get(luaGlue_, luaGlue_->state(), -1);
		}
		
		template<typename... _Args>
		void invokeVoidMethod(const std::string &name, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);

			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			pushInstance(luaGlue_->state(), obj);
			//printf("attempting to get method %s::%s\n", name_.c_str(), name.c_str());
			lua_getfield(luaGlue_->state(), -1, name.c_str());
			//lua_dump_stack(luaGlue_->state());
		
			lua_pushvalue(luaGlue_->state(), -2);
			applyTuple(luaGlue_, luaGlue_->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*luaGlue_, luaGlue_->state(), args), void(), '\0')... };
			
			lua_call(luaGlue_->state(), Arg_Count_+1, 0);
			//lua_pop(luaGlue_->state(), 1);
			lua_remove(luaGlue_->state(), -1);
		}
		
		LuaGlueClass<_Class> &pushInstance(_Class *obj)
		{
			return pushInstance(luaGlue_->state(), obj);
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, _Class *obj, bool owner = false)
		{
			LuaGlueObject<_Class> *udata = (LuaGlueObject<_Class> *)lua_newuserdata(state, sizeof(LuaGlueObject<_Class>));
			new (udata) LuaGlueObject<_Class>(obj, this, owner); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, const LuaGlueObject<_Class> &obj)
		{
			LuaGlueObject<_Class> *udata = (LuaGlueObject<_Class> *)lua_newuserdata(state, sizeof(LuaGlueObject<_Class>));
			new (udata) LuaGlueObject<_Class>(obj); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, std::shared_ptr<_Class> obj)
		{
			LG_Debug("pushInstance");

			if (obj == nullptr)
			{
				lua_pushnil(state);				
			}
			else
			{
				std::shared_ptr<_Class> *ptr_ptr = new std::shared_ptr<_Class>(obj);
				LuaGlueObject<std::shared_ptr<_Class>> *udata = (LuaGlueObject<std::shared_ptr<_Class>> *)lua_newuserdata(state, sizeof(LuaGlueObject<std::shared_ptr<_Class>>));
				new (udata) LuaGlueObject<std::shared_ptr<_Class>>(ptr_ptr, this, true); // placement new to initialize object
				
				luaL_getmetatable(state, name_.c_str());
				lua_setmetatable(state, -2);				
			}
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &ctor(const std::string &name)
		{
			//printf("ctor(%s)\n", name.c_str());
			auto impl = new LuaGlueCtorMethod<_Class, _Args...>(this, name.c_str());
			static_methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &dtor(void (_Class::*fn)())
		{
			//printf("dtor()\n");
			auto impl = new LuaGlueDtorMethod<_Class>(this, "m__gc", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__gc", impl);
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &index(_Value (_Class::*fn)(_Key))
		{
			//printf("index()\n");
			auto impl = new LuaGlueIndexMethod<_Value, _Class, _Key>(this, "m__index", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__index", impl);
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &newindex(void (_Class::*fn)(_Key, _Value))
		{
			//printf("newindex()\n");
			auto impl = new LuaGlueNewIndexMethod<_Value, _Class, _Key>(this, "m__newindex", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__newindex", impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type _Class::*prop)
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueDirectProperty<_Type, _Class>(this, name, prop);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &prop(const std::string &name, _Type _Class::*prop)
		{
			//printf("prop(%s)\n", name.c_str());
			auto impl = new LuaGlueDirectProperty<_Type, _Class>(this, name, prop);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*getter)(), void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*getter)())
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type(_Class::*getter)() const)
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter);
			properties_.addSymbol(name.c_str(), impl);

			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type(_Class::*getter)() const, void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class, void>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);

			return *this;
		}

        template<typename _Type>
        LuaGlueClass<_Class> &property(const std::string &name, _Type(_Class::*getter)() const, _Class* (_Class::*setter)(_Type))
        {
            //printf("property(%s)\n", name.c_str());
            auto impl = new LuaGlueProperty<_Type, _Class, _Class*>(this, name, getter, setter);
            properties_.addSymbol(name.c_str(), impl);

            return *this;
        }

        template<typename _Type>
        LuaGlueClass<_Class> &property(const std::string &name, _Type(_Class::*getter)() const, _Class& (_Class::*setter)(_Type))
        {
            //printf("property(%s)\n", name.c_str());
            auto impl = new LuaGlueProperty<_Type, _Class, _Class&>(this, name, getter, setter);
            properties_.addSymbol(name.c_str(), impl);

            return *this;
        }

        template<typename _Type>
        LuaGlueClass<_Class> &property(const std::string &name, _Type(_Class::*getter)() const, std::shared_ptr<_Class> (_Class::*setter)(_Type))
        {
            //printf("property(%s)\n", name.c_str());
            auto impl = new LuaGlueProperty<_Type, _Class, std::shared_ptr<_Class>>(this, name, getter, setter);
            properties_.addSymbol(name.c_str(), impl);

            return *this;
        }
		
		template<typename _Type>
		LuaGlueClass<_Class> &prop(const std::string &name, _Type (_Class::*getter)(), void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (_Class::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Ret, typename _BaseClass, typename... _Args>
		typename std::enable_if<std::is_convertible<_Class, _BaseClass>::value, LuaGlueClass<_Class>&>::type
		method(const std::string &name, _Ret (_BaseClass::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (_Class::*fn)(_Args...) const)
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueConstMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (_Class::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (_Class::*fn)(_Args...) const)
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueConstMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueStaticMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			static_methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

        template<typename _Ret, typename... _Args>
        LuaGlueClass<_Class> &methodWrapper(const std::string &name, _Ret(*fn)(_Class*, _Args...))
        {
            //printf("decorator method(%s)\n", name.c_str());
            auto impl = new LuaGlueStaticMethod<_Ret, _Class, _Class*, _Args...>(this, name, std::forward<decltype(fn)>(fn));
            methods.addSymbol(name.c_str(), impl);

            return *this;
        }

        template<typename _Ret, typename... _Args>
        LuaGlueClass<_Class> &methodWrapper(const std::string &name, _Ret(*fn)(std::shared_ptr<_Class>, _Args...))
        {
            //printf("decorator method(%s)\n", name.c_str());
            auto impl = new LuaGlueStaticMethod<_Ret, _Class, std::shared_ptr<_Class>, _Args...>(this, name, std::forward<decltype(fn)>(fn));
            methods.addSymbol(name.c_str(), impl);

            return *this;
        }
		
		bool hasMethod(const std::string& methodName)
		{
			auto state = luaGlue_->state();

			lua_getglobal(state, name_.c_str());
			if (!lua_istable(state, -1))
			{
				lua_pop(state, 1);

				return false;
			}

		    lua_getfield(state, -1, methodName.c_str());

		    bool exists = lua_isfunction(state, -1);

			lua_pop(state, 2);

		    return exists;
		}
		
		template<typename T>
		LuaGlueClass<_Class> &constant(const std::string &name, T v)
		{
			//printf("constant(%s)\n", name.c_str());
			auto impl = new LuaGlueConstant(name, v);
			constants_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &constants(const std::vector<LuaGlueConstant> &c)
		{
			for(unsigned int i = 0; i < c.size(); i++)
			{
				//printf("constant(%s)\n", c[i].name().c_str());
				auto impl = new LuaGlueConstant(c[i]);
				constants_.addSymbol(impl->name().c_str(), impl);
			}
			
			return *this;
		}
		
		LuaGlue &end() { return *(LuaGlue*)luaGlue_; }
		LuaGlueBase *luaGlue() { return luaGlue_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_createtable(luaGlue->state(), 0, 0);
			//int lib_id = lua_gettop(luaGlue->state());
			
			for(auto &method: static_methods)
			{
				//printf("Glue static method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &constant: constants_)
			{
				//printf("Glue constant: %s::%s\n", name_.c_str(), constant.first.c_str());
				if(!constant.ptr->glue(luaGlue))
					return false;
			}
			
			//printf("Glue Class: %s\n", name_.c_str());
			luaL_newmetatable(luaGlue->state(), name_.c_str());
			int meta_id = lua_gettop(luaGlue->state());
			
			int idx = luaGlue->getSymTab().findSym(name_.c_str()).idx;
			//printf("LuaGlueClass::glue: classidx: %i\n", idx);
			lua_pushinteger(luaGlue->state(), idx);
			lua_setfield(luaGlue->state(), meta_id, METATABLE_CLASSIDX_FIELD);
			
			//printf("LuaGlueClass::glue: intclassname: %s\n", typeid(_Class).name());
			lua_pushstring(luaGlue->state(), typeid(_Class).name());
			lua_setfield(luaGlue->state(), meta_id, METATABLE_INTCLASSNAME_FIELD);

			//printf("LuaGlueClass::glue: classname: %s\n", name_.c_str());
			lua_pushstring(luaGlue->state(), name_.c_str());
			lua_setfield(luaGlue->state(), meta_id, METATABLE_CLASSNAME_FIELD);
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_index, 1);
			lua_setfield(luaGlue->state(), meta_id, "__index");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_newindex, 1);
			lua_setfield(luaGlue->state(), meta_id, "__newindex");
			
			lua_pushvalue(luaGlue->state(), -2);
			lua_setfield(luaGlue->state(), meta_id, "__metatable");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_gc, 1);
			lua_setfield(luaGlue->state(), meta_id, "__gc");
			
			for(auto &method: methods)
			{
				//printf("Glue method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &method: meta_methods)
			{
				//printf("Glue method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &prop: properties_)
			{
				//printf("prop: %s: %p\n", prop.name, prop.ptr); 
				if(!prop.ptr->glue(luaGlue))
					return false;
			}
			
			//lua_pushvalue(luaGlue->state(), -1);
			lua_setmetatable(luaGlue->state(), -2);
			
			lua_pushvalue(luaGlue->state(), -1);
			lua_setglobal(luaGlue->state(), name_.c_str());
			
			lua_pop(luaGlue->state(), 1);

			//printf("done.\n");
			return true;
		}
		
		void _impl_dtor(std::shared_ptr<_Class> *)
		{
			
		}
		
		// LuaGlueObjectImpl dtor?
		void _impl_dtor(_Class *)
		{
			// ???
			LG_Debug("impl dtor!");
			//printf("LuaGlueClass<%s>::_impl_dtor\n", name_.c_str());
			//delete p;
		}
		
	private:
		LuaGlueBase *luaGlue_;
		std::string name_;
		
		LuaGlueSymTab<LuaGlueConstant *> constants_;
		LuaGlueSymTab<LuaGlueMethodBase *> methods;
		LuaGlueSymTab<LuaGlueMethodBase *> static_methods;
		LuaGlueSymTab<LuaGlueMethodBase *> meta_methods;
		//std::map<std::string, LuaGluePropertyBase *> properties_;
		LuaGlueSymTab<LuaGluePropertyBase *> properties_;
		
		// symbol lookup metamethod
		// TODO: make sure inheritance works
		
		// if we skip the metatable check, we can speed things up a bit,
		// but that would mean any lua sub classes won't get their props checked.
		int index(lua_State *state)
		{
			//printf("index!\n");
			
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);

				lua_getmetatable(state, 1);
				lua_pushvalue(state, 2); // push key
				
				lua_rawget(state, -2); // get function
				lua_remove(state, -2); // remove metatable
					
				if(properties_.exists(key))
				{
					//printf("prop!\n");
					lua_pushvalue(state, 1); // push args
					lua_pushvalue(state, 2);
					lua_call(state, 2, 1); // call function
				}
				
			}
			else if(type == LUA_TNUMBER)
			{
				lua_getmetatable(state, 1);
				lua_pushstring(state, "m__index");
				
				lua_rawget(state, -2); // get m__index method from metatable
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) { // if m__index is a function, call it
					lua_pushvalue(state, 1); // copy 1st and 2nd stack elements
					lua_pushvalue(state, 2);
					
					lua_call(state, 2, 1); // removes the argument copies
				// should always be a function.. might want to put some debug/trace messages to make sure
					
					//lua_dump_stack(state);
				}
			}
			else
			{
				printf("index: unsupported type: %i\n", type);
			}
			
			return 1;
		}
		
		int newindex(lua_State *state)
		{
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);

				lua_getmetatable(state, 1); // get metatable
				
				if(properties_.exists(key))
				{
					lua_pushvalue(state, 2); // push key
					lua_rawget(state, -2); // get field
					lua_remove(state, -2); // remove metatable
				
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
				
					lua_call(state, 3, 0); // call function from field above
					
					//lua_dump_stack(state);
				}
				else
				{
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
					
					//lua_dump_stack(state);
					lua_rawset(state, -3);
					//lua_dump_stack(state);
				}
				
				lua_pop(state, 1);
			}
			else if(type == LUA_TNUMBER)
			{
				lua_getmetatable(state, 1);
				
				lua_pushstring(state, "m__newindex");
				lua_rawget(state, -2); // get method
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) {
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push idx
					lua_pushvalue(state, 3); // push value
					lua_call(state, 3, 0);
					//lua_dump_stack(state);
				}
			}
			else
			{
				printf("newindex: unsupported type: %i\n", type);
			}
			
			//printf("newindex end!\n");
			
			return 0;
		}
		
		int gc(lua_State *state)
		{
			if(lua_isuserdata(state, -1))
			{
				LG_Debug("about to gc!");
				LuaGlueObjectBase *obj = (LuaGlueObjectBase *)lua_touserdata(state, -1);
				obj->put();
			}
			return 0;
		}
		
		static int lua_index(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->index(state);
		}
		
		static int lua_newindex(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->newindex(state);
		}
		
		static int lua_gc(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->gc(state);
		}
};

template<class _Class>
const char LuaGlueClass<_Class>::METATABLE_CLASSNAME_FIELD[] = "LuaGlueClassName";

template<class _Class>
const char LuaGlueClass<_Class>::METATABLE_INTCLASSNAME_FIELD[] = "LuaGlueIntClassName";

template<class _Class>
const char LuaGlueClass<_Class>::METATABLE_CLASSIDX_FIELD[] = "LuaGlueClassIdx";

#endif /* LUAGLUE_CLASS_H_GUARD */
