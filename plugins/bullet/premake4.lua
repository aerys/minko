newoption {
	trigger		= "with-bullet",
	description	= "Enable the Minko Bullet plugin."
}

minko.project.library "plugin-bullet"
	kind "StaticLib"
	language "C++"
	files { "**.hpp", "**.h", "**.cpp", "**.c" }
	includedirs {
		"src",
		"lib/bullet2/src"
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

	-- configurations
	configuration { "debug"}
		defines { "DEBUG" }
		flags { "Symbols" }
		targetdir "bin/debug"

	configuration { "release" }
		defines { "NDEBUG" }
		targetdir "bin/release"
		flags { "OptimizeSpeed" }
		
	-- linux
	configuration { "linux" }
		buildoptions { "-Wno-narrowing -Wno-int-to-pointer-cast" }

	-- windows
	configuration { "windows" }
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE"
		}

	-- macosx
	configuration { "macosx" }
		buildoptions { "-Wno-narrowing -Wno-int-to-pointer-cast" }

	-- emscripten
	configuration { "emscripten" }
		buildoptions { "-Wno-narrowing -Wno-int-to-pointer-cast" }
