-- A project defines one build target
project "minko-framework"
   kind "StaticLib"
   language "C++"
   files { "**.hpp", "**.cpp" }
   includedirs { "src" }
   targetdir "bin"

   configuration { "debug", "linux" }
      defines { "DEBUG" }
      flags { "Symbols" }
      links { "GL", "GLU", "glut" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"

   configuration { "release", "linux" }
      defines { "NDEBUG" }
      flags { "Optimize" }
      links { "GL", "GLU", "glut" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"

   configuration { "Debug", "x32", "windows" }
	  defines { "DEBUG" }
	  flags { "Symbols" }
	  includedirs { "lib/win32/includes" }
	  
   configuration { "Release", "x32", "windows" }
	  defines { "NDEBUG" }
	  flags { "Optimize" }
	  includedirs { "lib/win32/includes" }
	  
   configuration { "debug", "macosx" }
	  defines { "DEBUG" }
      buildoptions { "-std=c++11", "-stdlib=libc++" }
      linkoptions { "-std=c++11", "-stdlib=libc++" }
      libdirs { "/opt/local/lib/" }

   configuration { "release", "macosx" }
	  defines { "NDEBUG" }
	  buildoptions { "-std=c++11", "-stdlib=libc++" }
      linkoptions { "-std=c++11", "-stdlib=libc++" }
      libdirs { "/opt/local/lib/" }
