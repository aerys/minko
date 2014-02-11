PROJECT_NAME = path.getname(os.getcwd())

minko.project.worker("minko-worker-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include",
		"lib/curl/include"
	}

	defines { "CURL_STATICLIB" }

	-- linux
	configuration { "linux32 or linux64" }
		links { "curl"}

	-- windows
	configuration { "windows32 or windows 64" }
		links { "libcurl" }
		
	-- macos
	configuration { "osx64" }
		links { "curl"}
