local explode = {}

function explode.explode(delimiter, value)
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

return explode
