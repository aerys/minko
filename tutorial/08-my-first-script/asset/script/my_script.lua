-- /asset/script/my_script.lua
function my_script:start(node)
  print('start')
end
 
function my_script:update(node)
  print('update')
end
 
function my_script:stop(node)
  print('stop')
end