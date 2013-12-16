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

function rotate:start(node)
	self.oldX = self.mouse.x
	self.oldY = self.mouse.y
	self.rotationSpeedX = 0.
	self.rotationSpeedY = 0.
	self.distance = 3.
	self.lookAt = Vector3.create(0., 0., 0.)
	self.up = Vector3.create(0., 1., 0.)
	self.yaw = 0.
	self.pitch = math.pi * .5
	self.minPitch = 0.00001
	self.maxPitch = math.pi - self.minPitch
end

function rotate:update(node)
	local x = self.mouse.x
	local y = self.mouse.y
	if self.mouse.leftButtonIsDown then
		self.rotationSpeedX = self.rotationSpeedX + (x - self.oldX) / 1000
		self.rotationSpeedY = self.rotationSpeedY + (y - self.oldY) / 1000
	end
	self.oldX = x
	self.oldY = y

	self.yaw = self.yaw + self.rotationSpeedX
	self.rotationSpeedX = self.rotationSpeedX * .9

	self.pitch = self.pitch - self.rotationSpeedY
	self.pitch = math.min(math.max(self.pitch, self.minPitch), self.maxPitch)
	self.rotationSpeedY = self.rotationSpeedY * .9

	self:getModelToWorldMatrix(node):lookAt(
		self.lookAt,
		Vector3.create(
			self.lookAt.x + self.distance * math.cos(self.yaw) * math.sin(self.pitch),
			self.lookAt.y + self.distance * math.cos(self.pitch),
			self.lookAt.z + self.distance * math.sin(self.yaw) * math.sin(self.pitch)
		),
		self.up
	)
end
