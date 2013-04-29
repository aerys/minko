-- A project defines one build target
project "minko-framework"
   kind "StaticLib"
   language "C++"
   files { "**.hpp", "**.cpp" }
   includedirs { "src" }
   targetdir "bin"

   configuration "Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
      links { "GL", "GLU", "glut" }
      buildoptions "-std=c++0x"
      linkoptions "-std=c++0x"

   configuration "Release"
      defines { "NDEBUG" }
      flags { "Optimize" }
      links { "GL", "GLU", "glut" }
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