--[[
Copyright (c) 2015 Aerys

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

-- sensors plugin
minko.plugin.sensors = {}

function minko.plugin.sensors:enable()

	defines { "MINKO_PLUGIN_SENSORS" }
	
	minko.plugin.links { "sensors" }
	minko.plugin.enable { "sdl" }

	includedirs {
		minko.plugin.path("sensors") .. "/include"
	}

	configuration { "android", "SharedLib" }
		linkoptions {
			"-Wl,--undefined=Java_minko_plugin_sensors_AndroidAttitude_minkoNativeOnAttitudeEvent"
		}

	configuration { "ios" }
		links {
			"CoreMotion.framework" -- To receive data from mobile device sensors
		}
end

newoption {
	trigger		= "with-sensors",
	description	= "Enable the Minko sensors plugin."
}
