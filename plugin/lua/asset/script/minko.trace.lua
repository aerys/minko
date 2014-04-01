--[[
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]--

minko = minko or {}
minko.trace = {}

local mt = minko.trace

function mt.hierarchy(node)
	local function pretty(node, indent, last, args)
		args.output = args.output .. indent

		if last then
			args.output = args.output .. "+-"
			indent = indent .. "  "
		else
			args.output = args.output .. "|-"
			indent = indent .. "| "
		end

		args.output = args.output .. node:toString() .. "\n"

		local children = node.children

		for i = 1, children.size do
			pretty(children:at(i), indent, i == children.size, args)
		end
	end

	local args = {
		output = "",
		indent = ""
	}

	pretty(node, "", true, args)

	return args.output
end
