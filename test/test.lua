-- Test suite for the build system.

local luaunit = require('luaunit')
local ext = require('ext')

TestMinkoAction = {}

function TestMinkoAction:setUp()
	self.targetdir = 'bin/test'
end

function TestMinkoAction:testMinkoActionExists()
	luaunit.assertEquals(type(minko.action), 'table')
end

function TestMinkoAction:testCopyFile()
	os.mkdir(self.targetdir)
	os.execute(minko.action.copy('asset/effect/dummy.glsl', nil, self.targetdir))
	luaunit.assertTrue(os.isfile(path.join(self.targetdir, 'dummy.glsl')))
	os.rmdir(self.targetdir)
end

function TestMinkoAction:testCopyDirectory()
	os.mkdir(self.targetdir)
	os.execute(minko.action.copy('asset', nil, self.targetdir))
	luaunit.assertTrue(os.isdir(path.join(self.targetdir, 'asset')))
	luaunit.assertTrue(os.isdir(path.join(self.targetdir, 'asset', 'effect')))
	luaunit.assertTrue(os.isfile(path.join(self.targetdir, 'asset', 'effect', 'dummy.glsl')))
	os.rmdir(self.targetdir)
end

function TestMinkoAction:testCopyWildcardFile()
	os.mkdir(self.targetdir)
	os.execute(minko.action.copy('asset/effect/attribute/binding/*.effect', nil, self.targetdir))
	luaunit.assertTrue(os.isfile(path.join(self.targetdir, 'OneAttributeBinding.effect')))
	luaunit.assertTrue(os.isfile(path.join(self.targetdir, 'TwoAttributeBindings.effect')))
	os.rmdir(self.targetdir)
end

function TestMinkoAction:testCopyFileToDest()
	os.mkdir(path.join(self.targetdir, 'out'))
	os.execute(minko.action.copy('asset/effect/dummy.glsl', 'out', self.targetdir))
	luaunit.assertTrue(os.isfile(path.join(self.targetdir, 'out', 'dummy.glsl')))
	os.rmdir(self.targetdir)
end

function TestMinkoAction:tearDown()
	os.rmdir(self.targetdir)
end

luaunit.LuaUnit.run('TestMinkoAction')
