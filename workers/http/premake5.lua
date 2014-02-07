PROJECT_NAME = path.getname(os.getcwd())

minko.project.worker("minko-worker-" .. PROJECT_NAME)

	files {
		"include/**.hpp",
		"src/**.cpp"
	}

	includedirs {
		"include"
	}
