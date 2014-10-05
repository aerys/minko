PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c", "include/**.hpp" }

	includedirs {
		"include",
		"src"
	}

	minko.plugin.enable("zlib")

	--defines { "DEBUG_SKINNING" }

	-- assimp lib
	files {
		"lib/assimp/code/**.c",
		"lib/assimp/code/**.h",
		"lib/assimp/code/**.cpp",
		"lib/assimp/code/**.hpp",
		"lib/assimp/contrib/**.c",
		"lib/assimp/contrib/**.cc",
		"lib/assimp/contrib/**.h",
		"lib/assimp/contrib/**.cpp",
		"lib/assimp/contrib/**.hpp",
	}
	excludes {
		"lib/assimp/code/Assimp.cpp",
		"lib/assimp/code/STLExporter.cpp",
		"lib/assimp/code/PlyExporter.cpp",
		"lib/assimp/code/ColladaExporter.cpp",
		"lib/assimp/code/ObjExporter.cpp",
		"lib/assimp/code/Exporter.cpp",
		"lib/assimp/code/AssimpCExport.cpp"
	}

	includedirs {
		"lib/assimp",
		"lib/assimp/code",
		"lib/assimp/code/BoostWorkaround",
		"lib/assimp/include",
		"lib/assimp/contrib/clipper",
		"lib/assimp/contrib/ConvertUTF",
		"lib/assimp/contrib/irrXML",
		"lib/assimp/contrib/poly2tri",
		"lib/assimp/contrib/unzip"
		--"lib/assimp/contrib/zlib"
	}
	defines {
		"ASSIMP_BUILD_NO_OWN_ZLIB",
		"ASSIMP_BUILD_NO_EXPORT",
		"ASSIMP_BUILD_BOOST_WORKAROUND",
		"ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION",
	}

	configuration { "vs*" }
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_SCL_SECURE_NO_WARNINGS",
			"AI_LMW_MAX_WEIGHTS=4",
			-- OpenGL ES 2 max vertex count
			"AI_SLM_DEFAULT_MAX_VERTICES=128000",
			-- OpenGL ES 2 max indices/triangles count
			"AI_SLM_DEFAULT_MAX_TRIANGLES=21845"
		}

	configuration { "not vs*" }
		defines {
			'"AI_LMW_MAX_WEIGHTS=4"',
			-- OpenGL ES 2 max vertex count
			'"AI_SLM_DEFAULT_MAX_VERTICES=128000"',
			-- OpenGL ES 2 max indices/triangles count
			'"AI_SLM_DEFAULT_MAX_TRIANGLES=21845"'
		}

	configuration { "cc=clang" }
		buildoptions {
			"-Wno-unknown-warning-option",
			"-Wno-deprecated-register"
		}

	configuration { "osx64" }
		buildoptions {
			"-Wno-attributes"
		}

	configuration { "html5" }
		buildoptions {
			"-Wno-attributes",
			"-Wimplicit-function-declaration",
			"-Wparentheses-equality"
		}

	configuration { "android" }
		buildoptions {
			"-Wno-attributes"
		}
