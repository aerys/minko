-- A project defines one build target
project "minko-examples"
  kind "ConsoleApp"
  language "C++"
  links {
	"minko-jpeg",
	"minko-png",
	"minko-bullet",
	"minko-framework"
	--"minko-mk"
  }
  files { "**.hpp", "**.h", "**.cpp" }
  includedirs {
    "src",
	"lib/glfw/include",
    "../framework/src",
	"../plugins/jpeg/src",
	"../plugins/png/src",
	"../plugins/bullet/src"
    --"../plugins/mk/src"
  }

  configuration { "debug"}
    defines { "DEBUG" }
    flags { "Symbols" }
    targetdir "bin/debug"

  configuration { "release" }
    defines { "NDEBUG" }
    flags { "OptimizeSpeed" }
    targetdir "bin/release"

  -- linux
  configuration { "linux" }
    links { "GL", "GLU", "GLEW", "m" }
    buildoptions "-std=c++0x"
    linkoptions "-std=c++0x"

  -- windows
  configuration { "windows", "x32" }
    links { "OpenGL32", "glfw3dll", "glew32" }
  libdirs {
    "lib/glfw/bin/win32",
    "../framework/lib/glew/bin/win32"
  }

  -- macos
  configuration { "debug", "macosx" }
    buildoptions { "-std=c++11", "-stdlib=libc++" }
    linkoptions { "-std=c++11", "-stdlib=libc++" }
    libdirs { "/opt/local/lib/" }
    links { "GL", "GLU", "GLEW", "m" }
    includedirs { "/opt/local/include/" }
