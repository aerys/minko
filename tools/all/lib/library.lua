if _ACTION == 'clean' then
	local prjs = solution().projects
	for i, prj in ipairs(prjs) do
		os.rmdir(prj.basedir .. "/bin")
	end
end

function path.joinall(...)
  local s = ''
  for i, v in pairs(arg) do
    if i ~= 'n' then
      s = path.join(s, v)
    end
  end
  return s
end

--[[ prettyprint(struct, [limit], [indent])   Recursively print arbitrary data. 
  Set limit (default 100) to stanch infinite loops.
  Indents tables as [KEY] VALUE, nested tables as [KEY] [KEY]...[KEY] VALUE
  Set indent ("") to prefix each line:    Mytable [KEY] [KEY]...[KEY] VALUE
--]]
function prettyprint(s, l, i) -- recursive print (structure, limit, indent)
  l = (l) or 100; i = i or "";  -- default item limit, indent string
  if (l<1) then print "ERROR: Item limit reached."; return l-1 end;
  local ts = type(s);
  if (ts ~= "table") then print (i,ts,s); return l-1 end
  print (i,ts);           -- print "table"
  for k,v in pairs(s) do  -- print "[KEY] VALUE"
    l = prettyprint(v, l, i.."\t["..tostring(k).."]");
    if (l < 0) then break end
  end
  return l
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
