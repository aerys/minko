-- A project defines one build target
project "minko-examples"
  kind "ConsoleApp"
  language "C++"
  links {
	"minko-jpeg",
	"minko-framework"
  }
  files { "**.hpp", "**.h", "**.cpp" }
  includedirs {
    "src",
    "../framework/src",
	"../plugins/jpeg/src"
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
    links { "GL", "GLU", "glut", "m" }
    buildoptions "-std=c++0x"
    linkoptions "-std=c++0x"

  -- windows
  configuration { "windows", "x32" }
    links { "freeglut", "glew32" }
    libdirs { "../framework/lib/gl/win32/bin" }
    includedirs { "../framework/lib/gl/win32/includes" }

  -- macos
  configuration { "debug", "macosx" }
    buildoptions { "-std=c++11", "-stdlib=libc++" }
    linkoptions { "-std=c++11", "-stdlib=libc++" }
    libdirs { "/opt/local/lib/" }
    links { "GL", "GLU", "glut", "m" }
    includedirs { "/opt/local/include/" }
