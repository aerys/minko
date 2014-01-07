#ifndef LuaGlueUtils_H_GUARD
#define LuaGlueUtils_H_GUARD

#define lua_dump_stack(L) lua_dump_stack_(__LINE__, __PRETTY_FUNCTION__, L)
inline void lua_dump_stack_(int line, const char *func, lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	
	printf("%s:%i: lua_dump_stack: ", func, line);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

			case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

			default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf(", ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

inline void lua_dump_table(lua_State *L, int index)
{
	/* table is in the stack at index 'idx' */
	lua_pushvalue(L, index);

	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		
		lua_pushvalue(L, -2);
		
		printf("%s => ", lua_tostring(L, -1));
		int t = lua_type(L, -2);
		switch (t) {

			case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, -2));
			break;

			case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, -2) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, -2));
			break;

			default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf("\n");
		
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 2);
	}
	
	lua_pop(L, 1);
}

#endif /* LuaGlueUtils_H_GUARD */