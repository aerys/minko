function os.capture(cmd, raw)
	local f = assert(io.popen(cmd, 'r'))
	local s = assert(f:read('*a'))
	f:close()
	if not raw then
		s = string.gsub(s, '^%s+', '')
		s = string.gsub(s, '%s+$', '')
		s = string.gsub(s, '[\n\r]+', ' ')
	end
	return s
end

-- color
dofile('module/ext/color.lua')

 -- table.inspect
dofile('module/ext/inspect.lua')

function table.copy (t) -- shallow-copy a table
    if type(t) ~= "table" then return t end
    local meta = getmetatable(t)
    local target = {}
    for k, v in pairs(t) do target[k] = v end
    setmetatable(target, meta)
    return target
end

function table.clone (t) -- deep-copy a table
    if type(t) ~= "table" then return t end
    local meta = getmetatable(t)
    local target = {}
    for k, v in pairs(t) do
        if type(v) == "table" then
            target[k] = clone(v)
        else
            target[k] = v
        end
    end
    setmetatable(target, meta)
    return target
end

local function explode(delimiter, value)
	local t = {}
	local ll = 0
	if (#string == 1) then
		return { string }
	end
	while true do
		l = string.find(value, delimiter, ll, true) -- find the next d in the string
		if l ~= nil then -- if "not not" found then..
			table.insert(t, string.sub(value, ll, l-1)) -- Save it in our array.
			ll = l + 1 -- save just after where we found it for searching next time.
		else
			table.insert(t, string.sub(value, ll)) -- Save what's left in our array.
			break -- Break at end, as it should be, according to the lua manual.
		end
	end
	return t
end

function table.inject(obj, key, value)
	local keys = explode('.', key)
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
