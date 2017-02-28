-- ssl plugin
minko.plugin["ssl"] = {}

minko.plugin["ssl"].enable = function()
    defines { "MINKO_PLUGIN_SSL" }

    includedirs {
        minko.plugin.path("ssl") .. "/include",
        minko.plugin.path("ssl") .. "/lib/openssl/include"
    }

    configuration { "not html5" }
        minko.plugin.links { "ssl" }

    configuration { "linux" }
        links {
            "ssl",
            "crypto"
        }

    configuration { "ios" }
        libdirs {
            minko.plugin.path("ssl") .. "/lib/openssl/lib/ios"
        }
        links {
            "ssl",
            "crypto"
        }

    configuration { "android" }
        libdirs {
            minko.plugin.path("ssl") .. "/lib/openssl/lib/android"
        }
        links {
            "ssl",
            "crypto"
        }

    configuration { "osx64" }
        links {
            "ssl",
            "crypto"
        }

    configuration { "windows32" }
        defines {
            "OPENSSL_SYSNAME_WIN32"
        }

        links {
            minko.plugin.path("ssl") .. "/lib/openssl/lib/windows32/ssleay32",
            minko.plugin.path("ssl") .. "/lib/openssl/lib/windows32/libeay32"
        }
        prelinkcommands {
            minko.action.copy(minko.plugin.path("ssl") .. "/lib/openssl/lib/windows32/ssleay32.dll"),
            minko.action.copy(minko.plugin.path("ssl") .. "/lib/openssl/lib/windows32/libeay32.dll")
        }

    configuration { "windows64" }
        defines {
            "OPENSSL_SYSNAME_WIN32"
        }

        links {
            minko.plugin.path("ssl") .. "/lib/openssl/lib/windows64/ssleay32",
            minko.plugin.path("ssl") .. "/lib/openssl/lib/windows64/libeay32"
        }
        prelinkcommands {
            minko.action.copy(minko.plugin.path("ssl") .. "/lib/openssl/lib/windows64/ssleay32.dll"),
            minko.action.copy(minko.plugin.path("ssl") .. "/lib/openssl/lib/windows64/libeay32.dll")
        }
end
