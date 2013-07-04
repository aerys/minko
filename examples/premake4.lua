-- A project defines one build target
project "minko-examples"
  kind "ConsoleApp"
  language "C++"
  links {
<<<<<<< HEAD
	"minko-jpeg",
	"minko-png",
	"minko-bullet",
	"minko-framework"
	"minko-mk"
=======
    "minko-jpeg",
    "minko-png",
    "minko-mk",
    "minko-framework"
>>>>>>> 21ff4ee... Add glfw3 dependency on Linux and Mac OS X.
  }
  files { "**.hpp", "**.h", "**.cpp" }
  includedirs {
    "src",
<<<<<<< HEAD
	"lib/glfw/include",
=======
    "lib/glfw/include",
>>>>>>> 21ff4ee... Add glfw3 dependency on Linux and Mac OS X.
    "../framework/src",
	"../plugins/jpeg/src",
	"../plugins/png/src",
	"../plugins/bullet/src"
    "../plugins/mk/src"
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
    links { "GL", "glfw3", "m", "Xrandr", "Xxf86vm", "Xi", "rt" }
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
    links { "glfw3", "m", "Cocoa.framework", "OpenGL.framework", "IOKit.framework" }
    libdirs { "/opt/local/lib/" }
    includedirs { "/opt/local/include/" }
