project "minko-mk"
  kind "StaticLib"
  language "C++"
  files { "**.hpp", "**.h", "**.cpp", "**.c" }
  includedirs {
	"src"
	"../../framework/src"
  }
  links { "minko-framework" }
  
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
    buildoptions "-std=c++0x"
    linkoptions "-std=c++0x"

  -- windows
  configuration { "windows", "x32" }

  -- macos
  configuration { "debug", "macosx" }
    buildoptions { "-std=c++11", "-stdlib=libc++" }
    linkoptions { "-std=c++11", "-stdlib=libc++" }
    libdirs { "/opt/local/lib/" }
    includedirs { "/opt/local/include/" }
