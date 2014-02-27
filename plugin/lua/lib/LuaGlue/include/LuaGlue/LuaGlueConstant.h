#ifndef LUAGLUE_CONSTANT_H_GUARD
#define LUAGLUE_CONSTANT_H_GUARD

#include <string>
#include <stdexcept>
#include <lua.hpp>

#include "LuaGlue/LuaGlueBase.h"

class LuaGlueConstant
{
	public:
		enum {
			CTYPE_INT = 1,
			CTYPE_NUM,
			CTYPE_STR
		};
		
		LuaGlueConstant(const LuaGlueConstant &c) : name_(c.name_), type_(c.type_)
		{
			switch(type_)
			{
				case CTYPE_INT:
					int_value_ = c.int_value_;
					break;
					
				case CTYPE_NUM:
					num_value_ = c.num_value_;
					break;
					
				case CTYPE_STR:
					str_value_ = new std::string(c.str_value_->c_str());
					break;
					
				default:
					LG_Error("unknown constant type");
					break;
			}
		}
		
		LuaGlueConstant(const std::string &n, int v) : name_(n), int_value_(v), type_(CTYPE_INT) { }
		LuaGlueConstant(const std::string &n, double v) : name_(n), num_value_(v), type_(CTYPE_NUM) { }
		LuaGlueConstant(const std::string &n, const std::string &v) : name_(n), str_value_(new std::string(v)), type_(CTYPE_STR) { }
		~LuaGlueConstant()
		{
			if(type_ == CTYPE_STR)
				delete str_value_;
		}
		
		const int &intValue() const { if(type_ == CTYPE_INT) return int_value_; else throw std::runtime_error("invalid Constant type (int)"); }
		const double &numValue() const { if(type_ == CTYPE_NUM) return num_value_; else throw std::runtime_error("invalid Constant type (num)"); }
		const std::string &strValue() const { if(type_ == CTYPE_STR) return *str_value_; else throw std::runtime_error("invalid Constant type (str)"); }
		
		const std::string &name() const { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			switch(type_)
			{
				case CTYPE_INT:
					lua_pushinteger(luaGlue->state(), int_value_);
					break;
					
				case CTYPE_NUM:
					lua_pushnumber(luaGlue->state(), num_value_);
					break;
					
				case CTYPE_STR:
					lua_pushstring(luaGlue->state(), str_value_->c_str());
					break;
					
				default:
					LG_Error("unknown constant type");
					return false;
			}
			
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			
			return true;
		}
		
	private:
		std::string name_;
		union {
			int int_value_;
			double num_value_;
			std::string *str_value_;
		};
		int type_;
};

#endif /* LUAGLUE_CONSTANT_H_GUARD */
