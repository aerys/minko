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

rotate.MIN_PITCH = 0.00001
rotate.MAX_PITCH = math.pi - rotate.MIN_PITCH

function rotate:start(node)
	self.mouseHandler = coroutine.create(function() self:handleMouse(node) end)
	coroutine.resume(self.mouseHandler)
end

function rotate:applyRotation(node, lookAt, yaw, pitch, distance)
	getModelToWorldMatrix(node):lookAt(
		lookAt,
		Vector3.create(
			lookAt.x + distance * math.cos(yaw) * math.sin(pitch),
			lookAt.y + distance * math.cos(pitch),
			lookAt.z + distance * math.sin(yaw) * math.sin(pitch)
		),
		Vector3.up()
	)
end

function rotate:handleMouse(node)
	local lookAt = Vector3.create(0., 0., 0.)
	local distance = 3.
	local yaw = 0.
	local pitch = math.pi * .5
	local rotationSpeedX = 0.
	local rotationSpeedY = 0.

	self:applyRotation(node, lookAt, yaw, pitch, distance)

	while self.running do
		wait(getMouse().leftButtonDown)
		wait(getMouse().move)

		local oldX = getMouse().x
		local oldY = getMouse().y

		repeat
			local x = getMouse().x
			local y = getMouse().y
			
			if getMouse().leftButtonIsDown then
				rotationSpeedX = rotationSpeedX + (x - oldX) / 1000
				rotationSpeedY = rotationSpeedY + (y - oldY) / 1000
			end
			oldX = x
			oldY = y

			yaw = yaw + rotationSpeedX
			rotationSpeedX = rotationSpeedX * .9

			pitch = pitch - rotationSpeedY
			pitch = math.min(math.max(pitch, rotate.MIN_PITCH), rotate.MAX_PITCH)
			rotationSpeedY = rotationSpeedY * .9

			self:applyRotation(node, lookAt, yaw, pitch, distance)

			wait(getSceneManager().nextFrame)
		until
			not getMouse().leftButtonIsDown
			and math.abs(rotationSpeedX) < 0.001
			and math.abs(rotationSpeedY) < 0.001
	end
end
