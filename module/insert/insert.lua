local inspect = require 'inspect'
local explode = require 'explode'

local insert = {}

function insert.insert(obj, key, value)
	local keys = explode.explode('.', key)
	local last = #keys
	for i = 1, last do
		local key = keys[i]
		if i == last then
			obj[key] = value
		elseif not obj[key] then
			obj[key] = {}
		end
		obj = obj[key]
	end
end

local function test()
	local t = {}
	insert.insert(t, "a.b.c", 42)
	print(inspect.inspect(t))

	local t = { a = {} }
	insert.insert(t, "a.b.c", 42)
	print(inspect.inspect(t))

	local t = { a = { b = { c = 43 } } }
	insert.insert(t, "a.b.c", 42)
	print(inspect.inspect(t))
end

-- test()

return insert