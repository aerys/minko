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

minko = minko or {}
minko.coroutine = {}

local mc = minko.coroutine;

mc.WAITING_ON_TIME = {}
mc.WAITING_ON_SIGNAL = {}
mc.CURRENT_TIME = 0

function mc.wait(secondsOrSignal, ...)
    local co = coroutine.running()

    assert(co ~= nil, "The main thread cannot wait!")

    if type(secondsOrSignal) == 'number' then
        local wakeupTime = mc.CURRENT_TIME + secondsOrSignal

        mc.WAITING_ON_TIME[co] = wakeupTime
    else
        mc.WAITING_ON_SIGNAL[co] = {}

        local callback = function(...)
            for _, slot in ipairs(mc.WAITING_ON_SIGNAL[co]) do
                slot:disconnect()
            end
            mc.WAITING_ON_SIGNAL[co] = nil

            coroutine.resume(co, table.unpack({...}))
        end

        for _, signal in ipairs({secondsOrSignal, ...}) do
            table.insert(mc.WAITING_ON_SIGNAL[co], signal:connect(callback))
        end
    end

    return coroutine.yield(co)
end

function mc.wakeUpWaitingThreads(deltaTime)
    mc.CURRENT_TIME = mc.CURRENT_TIME + deltaTime

    local threadsToWake = {}
    for co, wakeupTime in pairs(mc.WAITING_ON_TIME) do
        if wakeupTime < mc.CURRENT_TIME then
            table.insert(threadsToWake, co)
        end
    end

    for _, co in ipairs(threadsToWake) do
        mc.WAITING_ON_TIME[co] = nil
        coroutine.resume(co)
    end
end

--[[
function mc.start(f, ...)
    local co = coroutine.create(function() f(...) end)

    return coroutine.resume(co)
end
]]

wait = mc.wait
wakeUpWaitingThreads = mc.wakeUpWaitingThreads
--coroutine.start = mc.start
