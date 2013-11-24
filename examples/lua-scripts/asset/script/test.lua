test = {
	counter = 0
}

function test:start(nodeName)
	print("test.start")
end

function test:update(nodeName)
	self.counter = self.counter + 1
	if (self.counter >= 100) then
		print("ok: " .. self.counter)
		self.counter = 0
	end
end

function test:stop(nodeName)
	print("test.stop")
end
