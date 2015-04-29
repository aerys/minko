-- Test suite for the build system.

local luaunit = require('luaunit')

TestMinkoAction = {}

function TestMinkoAction:testMinkoActionExists()
	luaunit.assertEquals(type(minko.action), "table")
end

luaunit.LuaUnit.run('TestMinkoAction')
