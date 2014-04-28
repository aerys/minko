table.inject(premake.tools.gcc, 'cppflags.system.linux', {
	"-MMD", "-MP"
})

table.inject(premake.tools.gcc, 'cppflags.system.macosx', {
	"-MMD", "-MP"
})

table.inject(premake.tools.gcc, 'cxxflags.system.linux', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'cxxflags.system.macosx', {
	"-std=c++11"
})

table.inject(premake.tools.gcc, 'tools.linux', {
	cc = 'gcc',
	cxx = MINKO_HOME .. '/tool/lin/script/g++.sh g++',
})

table.inject(premake.tools.gcc, 'tools.macosx', {
	cc = 'gcc',
	cxx = 'g++',
})
