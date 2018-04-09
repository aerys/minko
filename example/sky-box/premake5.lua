PROJECT_NAME = path.getname(os.getcwd())

minko.project.application("minko-example-" .. PROJECT_NAME)

    files {
        "src/**.cpp",
        "src/**.hpp",
        "asset/**"
    }

    -- plugins
    minko.plugin.enable("sdl")
    minko.plugin.enable("jpeg")
    minko.plugin.enable("fx")

    configuration { "html5" }
        minko.package.assets {
            ['**.jpg'] = { 'embed' }
        }
    configuration { "not html5" }
        minko.package.assets {
            ['**.jpg'] = { 'copy' }
        }