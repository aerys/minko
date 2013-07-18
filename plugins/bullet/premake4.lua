project "minko-bullet"
  kind "StaticLib"
  language "C++"
  files { "**.hpp", "**.h", "**.cpp", "**.c" }
  includedirs {
	"src",
	"lib/bullet2/src",
	"../../framework/src"
  }
  excludes { 
	"lib/bullet2/src/BulletMultiThreaded/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/DX11/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/MiniCL/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/Shared/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/GpuSoftBodySolvers/Shared/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/SpuNarrowPhaseCollisionTask/*.cpp",
	"lib/bullet2/src/BulletMultiThreaded/SpuSampleTask/*.h", 
	"lib/bullet2/src/BulletMultiThreaded/SpuSampleTask/*.cpp"
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
