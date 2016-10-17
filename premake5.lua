newoption {
	trigger	= 'no-example',
	description = 'Disable examples.'
}

newoption {
	trigger	= 'no-tutorial',
	description = 'Disable tutorials.'
}

newoption {
	trigger	= 'no-framework',
	description = 'Disable plugins.'
}

newoption {
	trigger	= 'no-plugin',
	description = 'Disable plugins.'
}

newoption {
	trigger	= 'no-test',
	description = 'Disable tests.'
}

newoption {
	trigger = 'dist-dir',
	description = 'Output folder for the redistributable SDK built with the \'dist\' action.'
}

newoption {
	trigger = 'platform',
	description = 'Platform for which we want to regroup the binaries with \'regroup\' action.'
}

newoption {
	trigger = 'config',
	description = 'Config for which we want to regroup the binaries with \'regroup\' action.'
}

newoption {
	trigger = 'type',
	description = 'Type of project we want to regroup the binaries with \'regroup\' action (among: example, tutorial, plugin).'
}

newoption {
	trigger = 'regroup-dir',
	description = 'Output folder where we want to regroup the binaries with \'regroup\' action.'
}

solution "minko"
	MINKO_HOME = path.getabsolute(os.getcwd())

	dofile('sdk.lua')

	-- buildable SDK
	MINKO_SDK_DIST = false

	-- framework
	if not _OPTIONS['no-framework'] then
		include 'framework'
	end

	-- plugin
	if not _OPTIONS['no-plugin'] then
		local plugins = minko.plugin.list()

		for _, plugin in ipairs(plugins) do
			if minko.plugin.requested(plugin) then
				include('plugin/' .. plugin)
			end
		end

		if string.find(_ACTION, 'xcode') then
			-- work around the inability of Xcode to build all
			-- projects if no dependency exists between them
			minko.project.library 'sdk'
				links { 'minko-framework' }

				for _, plugin in ipairs(plugins) do
					if minko.plugin.requested(plugin) then
						links { 'minko-plugin-' .. plugin }
					end
				end
		end
	end

	-- example
	if not _OPTIONS['no-example'] then
		local examples = {
			['assimp']				= true,
			['audio']				= true,
			['benchmark-cube']		= true,
			['blending']			= true,
			['cel-shading']         = true,
			['clone']				= false,
			['cube']				= true,
			['devil']				= false,
			['effect-config']		= false,
			['flares']				= false,
			['fog']					= true,
			['frustum']				= false,
			['hologram']			= false,
			['html-overlay']		= true,
			['http']				= true,
			['jobs']				= false,
			['joystick']			= true,
			['keyboard']			= false,
			['leap-motion']			= false,
			['light']				= true,
			['light-scattering']	= true,
			['line-geometry']		= false,
			['lua-scripts']			= minko.plugin.requested('lua'),
			['multi-surfaces']		= false,
			['nodejs']				= minko.plugin.requested('nodejs-worker'),
			['offscreen']			= minko.plugin.requested('offscreen'),
			['particles']			= minko.plugin.requested('particles'),
			['physics']				= true,
			['picking']				= true,
			['raycasting']			= false,
			['sensors']				= true,
			['serializer']			= true,
			['sky-box']				= true,
			['stencil']				= true,
			['visibility']			= false,
			['video-camera']		= true,
			['vr']				    = minko.plugin.requested('sensors'),
			['water']				= false,
			['shadow-mapping']		= true,
			['pbr']					= true,
		}

		for example, enabled in pairs(examples) do
			if enabled then
				include('example/' .. example)
			end
		end
	end

	-- tutorials
	if not _OPTIONS['no-tutorial'] then
		local tutorials = {
			['01-hello-cube']											= true,
			['02-handle-canvas-resizing']								= true,
			['03-rotating-the-camera-around-an-object-with-the-mouse']	= true,
			['04-moving-objects']										= true,
			['05-moving-objects-with-the-keyboard']						= true,
			['06-load-3d-files']										= true,
			['07-loading-scene-files']									= true,
			['10-working-with-the-basic-material']						= true,
			['11-working-with-the-phong-material']						= true,
			['12-working-with-normal-maps']								= true,
			['13-working-with-environment-maps']						= true,
			['14-working-with-specular-maps']							= true,
			['15-loading-and-using-textures']							= true,
			['16-loading-effects']										= true,
			['17-creating-a-custom-effect']								= true,
			['18-creating-custom-materials']							= true,
			['19-binding-the-model-to-world-transform']					= true,
			['20-binding-the-camera']									= true,
			['21-authoring-uber-shaders']								= true,
			['22-creating-a-simple-post-processing-effect']				= true,
			['23-using-external-glsl-code-in-effect-files']				= true,
			['24-working-with-custom-vertex-attributes']				= true,
			['25-working-with-ambient-lights']							= true,
			['26-working-with-directional-lights']						= true,
			['27-working-with-point-lights']							= true,
			['28-working-with-spot-lights']								= true,
			['29-hello-falling-cube']									= true,
			['30-applying-anti-aliasing-effect']						= true,
		}

		for tutorial, enabled in pairs(tutorials) do
			if enabled then
				include('tutorial/' .. tutorial)
			end
		end
	end

	-- test
	if not _OPTIONS['no-test'] then
		include 'test'
	end

newaction {
	trigger		= 'dist',
	description	= 'Generate the distributable version of the Minko SDK.',
	execute		= function()

		-- print("Building documentation...")
		-- os.execute("doxygen doc/Doxyfile")

		local distDir = 'dist'

		if _OPTIONS['dist-dir'] then
			distDir = _OPTIONS['dist-dir']
		end

		os.rmdir(distDir)

		os.mkdir(distDir)
		os.copyfile('sdk.lua', distDir .. '/sdk.lua')

		print("Packaging core framework...")

		-- framework
		os.mkdir(distDir .. '/framework')
		os.mkdir(distDir .. '/framework/bin')
		minko.os.copyfiles('framework/bin', distDir .. '/framework/bin')
		os.mkdir(distDir .. '/framework/include')
		minko.os.copyfiles('framework/include', distDir .. '/framework/include')
		os.mkdir(distDir .. '/framework/lib')
		minko.os.copyfiles('framework/lib', distDir .. '/framework/lib')
		os.mkdir(distDir .. '/framework/asset')
		minko.os.copyfiles('framework/asset', distDir .. '/framework/asset')

		-- skeleton
		os.mkdir(distDir .. '/skeleton')
		minko.os.copyfiles('skeleton', distDir .. '/skeleton')

		-- module
		os.mkdir(distDir .. '/module')
		minko.os.copyfiles('module', distDir .. '/module')

		-- -- doc
		-- os.mkdir(distDir .. '/doc')
		-- minko.os.copyfiles('doc/html', distDir .. '/doc')

		-- tool
		os.mkdir(distDir .. '/tool/')
		minko.os.copyfiles('tool', distDir .. '/tool')

		-- plugin
		local plugins = os.matchdirs('plugin/*')

		os.mkdir(distDir .. '/plugin')
		for i, basedir in ipairs(plugins) do
			local pluginName = path.getbasename(basedir)

			print('Packaging plugin "' .. pluginName .. '"...')

			-- plugins
			local dir = distDir .. '/plugin/' .. path.getbasename(basedir)
			local binDir = dir .. '/bin'

			-- plugin.lua
			assert(os.isfile(basedir .. '/plugin.lua'), 'missing plugin.lua')
			os.mkdir(dir)
			os.copyfile(basedir .. '/plugin.lua', dir .. '/plugin.lua')

			if minko.plugin[pluginName] and minko.plugin[pluginName].dist then
				minko.plugin[pluginName]:dist(dir)
			end

			-- bin
			if os.isdir(basedir .. '/bin') then
				os.mkdir(binDir)
				minko.os.copyfiles(basedir .. '/bin', binDir)
			end

			-- includes
			if os.isdir(basedir .. '/include') then
				os.mkdir(dir .. '/include')
				minko.os.copyfiles(basedir .. '/include', dir .. '/include')
			end

			-- assets
			if os.isdir(basedir .. '/asset') then
				os.mkdir(dir .. '/asset')
				minko.os.copyfiles(basedir .. '/asset', dir .. '/asset')
			end
		end

		os.execute(minko.action.zip(distDir, distDir .. '.zip'))
	end
}

newaction {
	trigger			= "doc",
	description		= "Create developer reference.",
	execute			= function()
		os.execute("doxygen doc/ref/Doxyfile")
	end
}

newaction {
	trigger			= "clean",
	description		= "Remove generated files.",
	execute			= function()
		os.execute(minko.action.clean("."))

		for _, pattern in ipairs { "framework", "plugin/*", "test", "example/*" } do
			local dirs = os.matchdirs(pattern)

			for _, dir in ipairs(dirs) do
				os.execute(minko.action.clean(dir))
			end
		end
	end
}

newaction {
	trigger			= "test",
	description		= "Run the automated test suite.",
	execute			= function()
		dofile "test/test.lua"
	end
}

newaction {
	trigger			= "regroup",
	description		= "Regroup all binaries into a single folder",
	execute			= function()

		function Set (list)
		  local set = {}
		  for _, l in ipairs(list) do set[l] = true end
		  return set
		end

		local availablePlatforms = Set { "windows32", "windows64", "linux32", "linux64", "osx64", "html5", "android", "ios" }
		local availableConfig = Set { "debug", "release"}
		local availableType = Set { "example", "tutorial", "plugin" }

		if _OPTIONS['platform'] and availablePlatforms[_OPTIONS['platform']] and
		   _OPTIONS['type'] and availableType[_OPTIONS['type']] and
		   _OPTIONS['config'] and availableConfig[_OPTIONS['config']] and
		   _OPTIONS['regroup-dir'] then

			local platform = _OPTIONS['platform']
			local projectType = _OPTIONS['type']
			local config = _OPTIONS['config']
			local outputDir = _OPTIONS['regroup-dir']

			local completeOutputDir = outputDir .. '/' .. platform .. '/' .. config .. '/' .. projectType
			os.mkdir(completeOutputDir)

			local dirs = os.matchdirs(projectType .. '/*')

			for i, basedir in ipairs(dirs) do

				local dirName = path.getbasename(basedir)
				local sourceDir = projectType .. '/' .. dirName .. '/bin/' .. platform .. '/' .. config
				if os.isdir(sourceDir) then
					print(dirName)

					os.mkdir(completeOutputDir .. '/' .. dirName)

					if platform == 'android' then
						if os.isdir(sourceDir .. '/bin/artifacts') then
							minko.os.copyfiles(sourceDir .. '/bin/artifacts', completeOutputDir .. '/' .. dirName)
						end
					else
						minko.os.copyfiles(sourceDir, completeOutputDir .. '/' .. dirName)
					end
				end
			end
		else
			print "Error: Some arguments are missing or are not correct. Please follow the usage."
			print "Usage: regroup --platform=$1 --config=$2 --type=$3 --regroup-dir=$4"
			print " \$1: the platform (windows32, windows64, linux32, linux64, osx64, html5, android or ios)"
			print " \$2: the configuration (debug or release)"
			print " \$3: the type of projects you want to pack (example, tutorial or plugin)"
			print " \$4: the output directory where you want to copy all files"
		end
	end
}
