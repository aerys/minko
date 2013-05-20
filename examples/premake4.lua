-- A project defines one build target
project "minko-examples"
   kind "ConsoleApp"
   language "C++"
   files { "**.hpp", "**.cpp" }
   includedirs { "src", "../framework/src" }
   targetdir "bin"

   configuration { "debug", "linux" }
      defines { "DEBUG" }
      flags { "Symbols" }
      links { "minko-framework", "GL", "GLU", "glut", "m" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"
      
   configuration { "release", "linux" }
      defines { "NDEBUG" }
      flags { "Optimize" }
      links { "minko-framework", "GL", "GLU", "glut", "m" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"
	  
   configuration { "debug", "x32", "windows" }
	  defines { "DEBUG" }
	  flags { "Symbols" }
	  links { "minko-framework", "freeglut", "glew32" }
	  libdirs { "../framework/lib/win32/bin" }
	  includedirs { "../framework/lib/win32/includes" }

	  
   configuration { "release", "x32", "windows" }
	  defines { "NDEBUG" }
	  flags { "Optimize" }
	  links { "minko-framework", "freeglut", "glew32" }
	  libdirs { "../framework/lib/win32/bin" }
	  includedirs { "../framework/lib/win32/includes" }

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
