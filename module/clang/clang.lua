table.inject(premake.tools.clang, 'cxxflags.system', {
	macosx = { "-MMD", "-MP", "-std=c++11", "-stdlib=libc++" }
})

table.inject(premake.tools.clang, 'ldflags.system', {
	macosx = { "-stdlib=libc++" }
})
