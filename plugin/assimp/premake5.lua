PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)
	kind "StaticLib"
	language "C++"
	files { "src/**.hpp", "src/**.h", "src/**.cpp", "src/**.c", "include/**.hpp" }
	includedirs {
		"include",
		"src"
	}
	
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
	includedirs {
		"lib/assimp/code",
		"lib/assimp/code/BoostWorkaround",
		"lib/assimp/include",
		"lib/assimp/contrib/clipper",
		"lib/assimp/contrib/ConvertUTF",
		"lib/assimp/contrib/irrXML",
		"lib/assimp/contrib/poly2tri",
		"lib/assimp/contrib/unzip",
		"lib/assimp/contrib/zlib"
	}
	defines {
		"ASSIMP_BUILD_BOOST_WORKAROUND",
		--"ASSIMP_BUILD_NO_X_IMPORTER",
		--"ASSIMP_BUILD_NO_OBJ_IMPORTER",
		--"ASSIMP_BUILD_NO_3DS_IMPORTER",
		--"ASSIMP_BUILD_NO_MD3_IMPORTER",
		--"ASSIMP_BUILD_NO_MD2_IMPORTER",
		--"ASSIMP_BUILD_NO_PLY_IMPORTER",
		"ASSIMP_BUILD_NO_MDL_IMPORTER",
		"ASSIMP_BUILD_NO_ASE_IMPORTER",
		"ASSIMP_BUILD_NO_HMP_IMPORTER",
		"ASSIMP_BUILD_NO_SMD_IMPORTER",
		"ASSIMP_BUILD_NO_MDC_IMPORTER",
		--"ASSIMP_BUILD_NO_MD5_IMPORTER",
		--"ASSIMP_BUILD_NO_STL_IMPORTER",
		--"ASSIMP_BUILD_NO_LWO_IMPORTER",
		"ASSIMP_BUILD_NO_DXF_IMPORTER",
		"ASSIMP_BUILD_NO_NFF_IMPORTER",
		"ASSIMP_BUILD_NO_RAW_IMPORTER",
		"ASSIMP_BUILD_NO_OFF_IMPORTER",
		"ASSIMP_BUILD_NO_AC_IMPORTER",
		"ASSIMP_BUILD_NO_BVH_IMPORTER",
		"ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
		"ASSIMP_BUILD_NO_IRR_IMPORTER",
		"ASSIMP_BUILD_NO_Q3D_IMPORTER",
		"ASSIMP_BUILD_NO_B3D_IMPORTER",
		--"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
		"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
		"ASSIMP_BUILD_NO_CSM_IMPORTER",
		"ASSIMP_BUILD_NO_3D_IMPORTER",
		--"ASSIMP_BUILD_NO_LWS_IMPORTER",
		--"ASSIMP_BUILD_NO_OGRE_IMPORTER",
		--"ASSIMP_BUILD_NO_MS3D_IMPORTER",
		"ASSIMP_BUILD_NO_COB_IMPORTER",
		--"ASSIMP_BUILD_NO_BLEND_IMPORTER",
		"ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
		"ASSIMP_BUILD_NO_NDO_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"ASSIMP_BUILD_NO_XGL_IMPORTER",
		--"ASSIMP_BUILD_NO_FBX_IMPORTER",
		"AI_LMW_MAX_WEIGHTS=4",
		-- OpenGL ES 2 max vertex count
		"AI_SLM_DEFAULT_MAX_VERTICES=128000",
		-- OpenGL ES 2 max indices/triangles count
		"AI_SLM_DEFAULT_MAX_TRIANGLES=21845"
	}

	configuration { "vs*" }
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_SCL_SECURE_NO_WARNINGS"
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
