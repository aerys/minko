-- color.lua
--
-- originally from:
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

color = {}

local ansi = not os.is("windows") or os.getenv('CMDER_ROOT') -- go, go, Cmder!
local esc = string.char(27, 91)

local names = {'black', 'red', 'green', 'yellow', 'blue', 'pink', 'cyan', 'white'}

color.fg = {}
color.bg = {}

for i, name in ipairs(names) do
    color.fg[name] = ansi and (esc .. tostring(30+i-1) .. 'm') or ''
    color.bg[name] = ansi and (esc .. tostring(40+i-1) .. 'm') or ''
    color.fg[name:upper()] = ansi and (esc .. tostring(90+i-1) .. 'm') or ''
    color.bg[name:upper()] = ansi and (esc .. tostring(100+i-1) .. 'm') or ''
end

color.reset = ansi and (esc .. '0m') or ''
color.clear = ansi and (esc .. '2J') or ''

color.bold = ansi and (esc .. '1m') or ''
color.faint = ansi and (esc .. '2m') or ''
color.normal = ansi and (esc .. '22m') or ''
color.invert = ansi and (esc .. '7m') or ''
color.underline = ansi and (esc .. '4m') or ''

color.hide = ansi and (esc .. '?25l') or ''
color.show = ansi and (esc .. '?25h') or ''

function color.test()
    print(color.reset .. color.bg.green .. color.fg.RED .. "This is bright red on green" .. color.reset)
    print(color.invert .. "This is inverted..." .. color.reset .. " And this isn't.")
    print(color.fg(0xDE) .. color.bg(0xEE) .. "You can use xterm-256 colors too!" .. color.reset)
    print("And also " .. color.bold .. "BOLD" .. color.normal .. " if you want.")
    print(color.bold .. color.fg.BLUE .. color.bg.blue .. "Miss your " .. color.fg.RED .. "C-64" .. color.fg.BLUE .. "?" .. color.reset)
end

-- color.test()
