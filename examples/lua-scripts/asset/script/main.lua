--[[
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
]]--

function main:start(root)
	local assets = 	getSceneManager().assets

	assets
		:queue('effect/Basic.effect')
		:queue('effect/Phong.effect')
		:queue('texture/box.png')
		:queue('script/rotate.lua')
		:queue('script/framerate.lua')
		:load()

	--root:addComponent(assets:script('script/framerate.lua'))

	self.camera = Node.create()
		:addComponent(Renderer.create())
		:addComponent(Transform.createFromMatrix(Matrix4x4.create():lookAt(
			Vector3.zero(),
			Vector3.create(0., 0., 3.),
			Vector3.up()
		)))
		:addComponent(PerspectiveCamera.create(800. / 600., math.pi * .25, .1, 1000.))
		:addComponent(assets:script('script/rotate.lua'))
		:addComponent(DirectionalLight.create(.4))
	root:addChild(self.camera)

	local lights = Node.create()
		:addComponent(AmbientLight.create(.2))
		:addComponent(DirectionalLight.create(.4))
	root:addChild(lights)

	local cube = Node.create()
		:addComponent(Transform.create())
		:addComponent(Surface.create(
			CubeGeometry.create(assets.context),
			Material.create():setTexture('diffuseMap', assets:texture('texture/box.png')),
			assets:effect('effect/Phong.effect')
		))
	root:addChild(cube)
end
