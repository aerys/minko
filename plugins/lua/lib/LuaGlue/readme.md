LuaGlue
=======

LuaGlue is a C++11 template based binding library.

It is contained entirely in headers, with no code to link to.

License: Zlib

example
-------

Lua Code:
```lua
local test = Foo.new(333);
test:abc(1,2,3);

print("ONE: "..Foo.ONE);
print("TWO: "..Foo.TWO);
print("THREE: "..Foo.THREE);
```

C++ Code
```cpp
#include <LuaGlue/LuaGlue.h>

class Foo
{
	public:
		Foo(int i) { printf("ctor! %i\n", i); }
		~Foo();
		
		int abc(int a, int b, int c) { printf("%i:%i:%i\n", a,b,c); return 143; }
		static void aaa() { printf("aaa!\n"); }
};

int main(int, char **)
{
	LuaGlue state;
	
	state.
		Class<Foo>("Foo").
			ctor<int>("new").
			method("abc", &Foo::abc).
			method("aaa", &Foo::aaa).
			constants( { { "ONE", 1 }, { "TWO", 2.0 }, { "THREE", "three" } } ).
		end().open().glue();
	
	if(luaL_dofile(state.state(), "foo.lua"))
	{
		printf("failed to dofile: foo.lua\n");
		const char *err = luaL_checkstring(state.state(), -1);
		printf("err: %s\n", err);
	}
		
	return 0;
}
```

LICENSE
=======

> Copyright (c) 2013 Thomas Fjellstrom
>
> This software is provided 'as-is', without any express or implied
> warranty. In no event will the authors be held liable for any damages
> arising from the use of this software.

> Permission is granted to anyone to use this software for any purpose,
> including commercial applications, and to alter it and redistribute it
> freely, subject to the following restrictions:
>
>   1. The origin of this software must not be misrepresented; you must not
>   claim that you wrote the original software. If you use this software
>   in a product, an acknowledgment in the product documentation would be
>   appreciated but is not required.
>
>   2. Altered source versions must be plainly marked as such, and must not be
>   misrepresented as being the original software.
>
>   3. This notice may not be removed or altered from any source
>   distribution.
