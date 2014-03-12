#ifndef LUAGLUE_FUNCTION_BASE_H_GUARD
#define LUAGLUE_FUNCTION_BASE_H_GUARD

class LuaGlueBase;

class LuaGlueFunctionBase
{
	public:
		LuaGlueFunctionBase() { }
		virtual ~LuaGlueFunctionBase() { }
		virtual bool glue(LuaGlueBase *luaGlue) = 0;
};

#endif /* LUAGLUE_FUNCTION_BASE_H_GUARD */
