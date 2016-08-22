PROJECT_NAME = path.getname(os.getcwd())

minko.project.library("minko-plugin-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp",
		"src/**.hpp"
	}

	includedirs {
		"include",
		"src"
	}

	configuration { "html5" }
		excludes {
			"src/minko/net/NativeWebSocketImpl.hpp",
			"src/minko/net/NativeWebSocketImpl.cpp"
		}
		files {
			"src/minko/net/EmscriptenWebSocketImpl.hpp",
			"src/minko/net/EmscriptenWebSocketImpl.cpp"
		}

	configuration { "not html5" }
		excludes {
			"src/minko/net/EmscriptenWebSocketImpl.hpp",
			"src/minko/net/EmscriptenWebSocketImpl.cpp"
		}
		-- websocket++
		files {
			"lib/websocketpp/**.hpp",
			"lib/websocketpp/**.hpp"
		}
		includedirs {
			"lib/websocketpp",
		}
		-- asio
		files {
			"lib/asio/include/**.hpp",
		}
		includedirs {
			"lib/asio/include"
		}
		defines {
			"ASIO_STANDALONE",
		}
		-- openssl
		files {
			"lib/openssl/include/**.h"
		}


	configuration { "linux32 or linux64" }
		-- openssl
		includedirs {
			"/usr/include/openssl"
		}

	configuration { "android" }
		-- openssl
		includedirs {
			"lib/openssl/include"
		}
		libdirs {
			"lib/openssl/lib/android"
		}

	configuration { "windows" }
		-- websocket++
		defines {
			"_WEBSOCKETPP_CPP11_INTERNAL_",
		}
		-- asio
		defines {
			"_WIN32_WINNT=0x0501"
		}
		-- openssl
		defines {
			"OPENSSL_SYSNAME_WIN32"
		}
		includedirs {
			"lib/openssl/include"
		}
