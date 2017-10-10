--[[
Copyright (c) 2016 Aerys

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

-- websocket plugin
minko.plugin["websocket"] = {}

minko.plugin["websocket"].enable = function()
    configuration { "not html5" }
        minko.plugin.enable("ssl")

        -- websocket++
        includedirs {
            minko.plugin.path("websocket") .. "/lib/websocketpp",
        }
        -- asio
        includedirs {
            minko.plugin.path("websocket") .. "/lib/asio/include"
        }
        defines {
            "ASIO_STANDALONE",
        }

    configuration { "windows" }
        -- websocket++
        defines {
            "_WEBSOCKETPP_CPP11_INTERNAL_",
        }
        -- asio
        defines {
            "_WIN32_WINNT=0x0501"
        }

    configuration { }

    minko.plugin.links { "websocket" }

    includedirs { minko.plugin.path("websocket") .. "/include" }

    defines { "MINKO_PLUGIN_WEBSOCKET" }
end
