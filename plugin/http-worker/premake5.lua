PROJECT_NAME = path.getname(os.getcwd())

if not minko.platform.supports { "windows32", "windows64", "linux32", "linux64", "osx64", "ios", "android" } then
	return
end

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
		links { "curl", "Security.framework"}
		minko.plugin.links { "zlib" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/osx64/release" }
		
	configuration { "android" }
		links { "curl" }
		minko.plugin.links { "zlib" }
		libdirs { minko.plugin.path("http-worker") .. "/lib/curl/lib/android/release" }