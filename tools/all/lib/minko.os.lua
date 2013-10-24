minko.os = {}

minko.os.copyfiles = function(src, dst)
  assert(os.isdir(src), 'bad argument #1 to os.copyfiles (directory expected)')
  assert(os.isdir(dst), 'bad argument #2 to os.copyfiles (directory expected)')
  local list = os.matchfiles(path.join(src, '**'))
  for _, v in pairs(list) do
    printf(v)
    printf(path.getrelative(path.getdirectory(v), src))
    local dir = path.join(dst, path.getrelative(path.getdirectory(v), src))
    os.mkdir(dir)
    os.copyfile(v, dir)
  end
end
