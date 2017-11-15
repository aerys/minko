PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

    files {
      "src/**.cpp",
      "include/**.hpp",
      "lib/freetype/**.h",
      "lib/freetype/**.c"
    }

    includedirs {
      "include",
      "lib/freetype/include/freetype2",
      "lib/freetype/include"
  }

    configuration { "windows or linux" }
        defines { "MINKO_PLUGIN_TTF_FREETYPE" }
