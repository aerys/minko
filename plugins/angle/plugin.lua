--[[
Copyright (c) 2013 Aerys

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

-- angle plugin
minko.plugin.angle = {}

function minko.plugin.angle:enable()
	local kind = configuration().kind

	assert(os.get() == 'windows', "The ANGLE plugin is compatible only with Windows.")

	configuration { "windows32 or windows64" }
		defines { "MINKO_ANGLE" }

		libdirs { minko.plugin.path("angle") .. "/lib/win/ANGLE/lib" }
		links { "libGLESv2", "libEGL" }
		includedirs { minko.plugin.path("angle") .. "/lib/win/ANGLE/include" }
		
		if kind ~= "StaticLib" and kind ~= "SharedLib" then
			postbuildcommands {
				minko.action.copy(minko.plugin.path("angle") .. "/lib/win/ANGLE/lib/*.dll")
			}
		end
end
