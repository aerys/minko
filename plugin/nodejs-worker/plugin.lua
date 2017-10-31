-- nodejs worker plugin
minko.plugin["nodejs-worker"] = {}

minko.plugin["nodejs-worker"].enable = function (self)
    includedirs { minko.plugin.path("nodejs-worker") .. "/include" }

    defines { "MINKO_PLUGIN_NODEJS_WORKER" }

    configuration { "not html5" }
        minko.plugin.enable("ssl")

    configuration { "android" }
        minko.plugin.links { "nodejs-worker" }

        links {
            "node"
        }

        libdirs {
            minko.plugin.path("nodejs-worker") .. "/lib/nodejs/android"
        }

        prelinkcommands {
            minko.action.copy(minko.plugin.path("nodejs-worker") .. "/lib/nodejs/android/*.so")
        }

end
