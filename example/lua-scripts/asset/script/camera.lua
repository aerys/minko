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

camera.START_YAW = math.pi * .4
camera.START_PITCH = math.pi * .45
camera.MIN_PITCH = 0.00001
camera.MAX_PITCH = math.pi - camera.MIN_PITCH
camera.INERTIA_FACTOR = 0.8
camera.Z_FAR = 1000.
camera.MAX_SCALE = 25.

function camera:start(node)
	if getOption('--vr') then
		return
	end

	local canvas = getCanvas()
	local cameraComp = PerspectiveCamera.create(canvas.width / canvas.height, math.pi * .25, .1, camera.Z_FAR)

	node.name = 'mainCamera'
	node
		:addComponent(Renderer.create())
		:addComponent(Transform.create())
		:addComponent(cameraComp)

	self.canvasResizedSlot = canvas.resized:connect(function(canvas, width, height)
		cameraComp.aspectRatio = width / height
	end)

	self:handleMouseMove(node)
end

function camera:applyRotation(node, lookAt, yaw, pitch, distance)
	node:getTransform().matrix:lookAt(
		lookAt,
		Vector3.create(
			lookAt.x + distance * math.cos(yaw) * math.sin(pitch),
			lookAt.y + distance * math.cos(pitch),
			lookAt.z + distance * math.sin(yaw) * math.sin(pitch)
		),
		Vector3.up()
	)
end

camera.handleMouseMove = coroutine.wrap(function(self, node)
	local yaw = camera.START_YAW
	local pitch = camera.START_PITCH
	local rotationSpeedX = 0.
	local rotationSpeedY = 0.
	local zoomSpeed = 0.
	local lookAt = Vector3.zero()
	local distance = 3.
	local mouse = getCanvas().mouse

	self.mouseWheelSlot = getCanvas().mouse.wheel:connect(function(m, h, v)
		zoomSpeed = zoomSpeed + v * 0.1
	end)
	
	self:applyRotation(node, lookAt, yaw, pitch, distance)

	while self.running do
		wait(mouse.leftButtonDown, mouse.wheel)
		
		local oldX = mouse.x
		local oldY = mouse.y

		repeat
			local x = mouse.x
			local y = mouse.y
			
			if mouse.leftButtonIsDown then
				rotationSpeedX = rotationSpeedX + (x - oldX) / 1000
				rotationSpeedY = rotationSpeedY + (y - oldY) / 1000
			end
			oldX = x
			oldY = y

			yaw = yaw + rotationSpeedX
			rotationSpeedX = rotationSpeedX * camera.INERTIA_FACTOR

			pitch = pitch - rotationSpeedY
			pitch = math.min(math.max(pitch, camera.MIN_PITCH), camera.MAX_PITCH)
			rotationSpeedY = rotationSpeedY * camera.INERTIA_FACTOR

			distance = distance + zoomSpeed
			distance = math.max(distance, .5)
			zoomSpeed = zoomSpeed * camera.INERTIA_FACTOR

			self:applyRotation(node, lookAt, yaw, pitch, distance)

			wait(getSceneManager().nextFrame)
		until
			not mouse.leftButtonIsDown
			and math.abs(rotationSpeedX) < 0.001
			and math.abs(rotationSpeedY) < 0.001
			and math.abs(zoomSpeed) < 0.001
	end
end)
