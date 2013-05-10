-- A project defines one build target
project "minko-examples"
   kind "ConsoleApp"
   language "C++"
   files { "**.hpp", "**.cpp" }
   includedirs { "src", "../framework/src" }
   targetdir "bin"

   configuration "Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
      links { "minko-framework", "GL", "GLU", "glut", "m" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"
      
   configuration "Release"
      defines { "NDEBUG" }
      flags { "Optimize" }
      links { "minko-framework", "GL", "GLU", "glut", "m" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"

   configuration "macosx"
      buildoptions
      {
         "-std=c++11",
         "-stdlib=libc++"
      }
      linkoptions
      {
         "-std=c++11",
         "-stdlib=libc++"
      }
      libdirs
      {
         "/opt/local/lib/"
      }
