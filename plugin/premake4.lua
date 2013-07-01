-- A project defines one build target
project "minko-plugin"
  kind "StaticLib"
  language "C++"
  files { "**.hpp", "**.cpp", "**.h" }
  includedirs {
    "src",
    "../framework/src"
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
    links { "minko-framework", "GL", "GLU", "glut", "m" }
    buildoptions "-std=c++0x"
    linkoptions "-std=c++0x"

  -- windows
  configuration { "windows", "x32" }
    links { "minko-framework", "freeglut", "glew32" }
    libdirs { "../framework/lib/gl/win32/bin" }
    includedirs { "../framework/lib/gl/win32/includes" }

  -- macos
  configuration { "debug", "macosx" }
    buildoptions { "-std=c++11", "-stdlib=libc++" }
    linkoptions { "-std=c++11", "-stdlib=libc++" }
    libdirs { "/opt/local/lib/" }
    links { "minko-framework", "GL", "GLU", "glut", "m" }
    includedirs { "/opt/local/include/" }
