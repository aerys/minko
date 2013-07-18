function path.joinall(...)
  local s = ''
  for i, v in pairs(arg) do
    if i ~= 'n' then
      s = path.join(s, v)
    end
  end
  return s
end

function printt(table)
  if not #table then
    io.write('{}')
  else
    io.write('{')
    io.write(tostring(table[1]))
    for i = 2, #table do
      io.write(', ')
      io.write(tostring(table[i]))
    end
    printf('}')
  end
end

function os.copyfiles(src, dst)
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

function newplatform(platform)
  local name = platform.name
  local description = platform.description
 
  -- Register new platform
  premake.platforms[name] = {
    cfgsuffix = "_"..name,
    iscrosscompiler = true
  }
 
  -- Allow use of new platform in --platfroms
  table.insert(premake.option.list["platform"].allowed, { name, description })
  table.insert(premake.fields.platforms.allowed, name)
 
  -- Add compiler support
  -- gcc
  premake.gcc.platforms[name] = platform.gcc
  --other compilers (?)
end
