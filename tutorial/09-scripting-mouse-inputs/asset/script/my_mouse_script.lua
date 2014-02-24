-- /asset/script/my_mouse_script.lua

function my_mouse_script:start(node)
  self.co = coroutine.create(my_mouse_script.handleMouseDown)
  coroutine.resume(self.co, self)
end

function my_mouse_script:handleMouseDown()
  print("please click...")
  -- execution of this specific script will pause when it hits the call to wait()
  wait(getCanvas().mouse.leftButtonDown)
  -- execution will resume here when the mouse.leftButtonDown has been executed
  print("left button down!")
end