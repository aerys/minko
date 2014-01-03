-- color.lua

--------------------------------------------------------------------------------

-- A super-simple way to make colored text output in Lua.
-- To use, simply print out things from this module, then print out some text.
--
-- Example:
-- print(color.bg.green .. color.fg.RED .. "This is bright red on green")
-- print(color.invert .. "This is inverted..." .. color.reset .. " And this isn't.")
-- print(color.fg(0xDE) .. color.bg(0xEE) .. "You can use xterm-256 colors too!" .. color.reset)
-- print("And also " .. color.bold .. "BOLD" .. color.normal .. " if you want.")
-- print(color.bold .. color.fg.BLUE .. color.bg.blue .. "Miss your " .. color.fg.RED .. "C-64" .. color.fg.BLUE .. "?" .. color.reset)
--
-- You can see all these examples in action by calling color.test()
--
-- Can't pick a good color scheme? Look at a handy chart:
-- print(color.chart())
--
-- If you want to add anything to this, check out the Wikipedia page on ANSI control codes:
-- http://en.wikipedia.org/wiki/ANSI_escape_code

--------------------------------------------------------------------------------

-- Copyright (C) 2012 Ross Andrews
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU Lesser General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/lgpl.txt>.

--------------------------------------------------------------------------------

-- A note about licensing:
--
-- The LGPL isn't really intended to be used with non-compiled libraries. The way
-- I interpret derivative works of this library is this: if you don't modify this
-- file, and the program it's embedded in doesn't modify the Lua table it defines,
-- then you can distribute it with a program under any license. If you do either
-- of those things, then you've created a derivative work of this library and you
-- have to release the modifications you made under this same license.

module(..., package.seeall)

local esc = string.char(27, 91)

local names = {'black', 'red', 'green', 'yellow', 'blue', 'pink', 'cyan', 'white'}
local hi_names = {'BLACK', 'RED', 'GREEN', 'YELLOW', 'BLUE', 'PINK', 'CYAN', 'WHITE'}

fg, bg = {}, {}

for i, name in ipairs(names) do
   fg[name] = esc .. tostring(30+i-1) .. 'm'
   _M[name] = fg[name]
   bg[name] = esc .. tostring(40+i-1) .. 'm'
end

for i, name in ipairs(hi_names) do
   fg[name] = esc .. tostring(90+i-1) .. 'm'
   _M[name] = fg[name]
   bg[name] = esc .. tostring(100+i-1) .. 'm'   
end

local function fg256(_,n)
   return esc .. "38;5;" .. n .. 'm'   
end

local function bg256(_,n)
   return esc .. "48;5;" .. n .. 'm'   
end

setmetatable(fg, {__call = fg256})
setmetatable(bg, {__call = bg256})

reset = esc .. '0m'
clear = esc .. '2J'

bold = esc .. '1m'
faint = esc .. '2m'
normal = esc .. '22m'
invert = esc .. '7m'
underline = esc .. '4m'

hide = esc .. '?25l'
show = esc .. '?25h'

function move(x, y)
   return esc .. y .. ';' .. x .. 'H'
end

home = move(1, 1)

--------------------------------------------------

function chart(ch,col)
   local cols = '0123456789abcdef'

   ch = ch or ' '
   col = col or fg.black
   local str = color.reset .. color.bg.WHITE .. col

   for y = 0, 15 do
      for x = 0, 15 do
         local lbl = cols:sub(x+1, x+1)
         if x == 0 then lbl = cols:sub(y+1, y+1) end

         str = str .. color.bg.black .. color.fg.WHITE .. lbl
         str = str .. color.bg(x+y*16) .. col .. ch
      end
      str = str .. color.bg.black .. "\n"
   end
   return str .. color.reset
end

function test()
   print(color.reset .. color.bg.green .. color.fg.RED .. "This is bright red on green" .. color.reset)
   print(color.invert .. "This is inverted..." .. color.reset .. " And this isn't.")
   print(color.fg(0xDE) .. color.bg(0xEE) .. "You can use xterm-256 colors too!" .. color.reset)
   print("And also " .. color.bold .. "BOLD" .. color.normal .. " if you want.")
   print(color.bold .. color.fg.BLUE .. color.bg.blue .. "Miss your " .. color.fg.RED .. "C-64" .. color.fg.BLUE .. "?" .. color.reset)
   print("Try printing " .. color.underline .. _M._NAME .. ".chart()" .. color.reset)
end