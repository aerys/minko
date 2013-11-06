minko.path = {}

minko.path.joinall = function(...)
  local s = ''
  for i, v in pairs(arg) do
    if i ~= 'n' then
      s = path.join(s, v)
    end
  end
  return s
end
