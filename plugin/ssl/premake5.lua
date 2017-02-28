newoption {
    trigger            = "rebuild-openssl",
    description        = "Rebuilds OpenSSL for the specified platform."
}

if _OPTIONS["rebuild-openssl"] then
    -- include "lib/openssl"
    error("not implemented, see README")
end

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64", "ios", "android" } then
    return
end

PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

    removeplatforms { "html5" }

    files {
        "include/**.hpp",
        "src/**.cpp",
        "src/**.hpp"
    }
    includedirs {
        "include",
        "lib/openssl/include",
        "src"
    }
