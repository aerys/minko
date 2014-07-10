PROJECT_NAME = path.getname(os.getcwd())

minko.project.worker("minko-plugin-" .. PROJECT_NAME)

	removeplatforms { "html5" }

	files {
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include",
		"lib/curl/include"
	}

	defines { "CURL_STATICLIB" }

	configuration { "linux32 or linux64" }
		links { "curl"}

	configuration { "windows32 or windows64" }
		links { "libcurl" }

	configuration { "osx64" }
		links { "curl",  "minko-plugin-zlib", "Security.framework"}
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/osx64/release" }