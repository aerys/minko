#pragma once

#include "minko/Common.hpp"

namespace minko
{
    namespace animation
    {
        uint
        getTimeInRange(int time, uint duration);

        template<typename T>
        uint
        getIndexForTime(uint time, const std::vector<std::pair<uint,T>>& timetable);
    }

    uint
    animation::getTimeInRange(int time, uint duration)
    {
        const uint t = (uint)duration > 0
        ? ( time >= 0
            ? time % duration
            : ((time % duration) + duration) % duration )
        : 0;

        assert(t < duration);

        return t;
    }

    template<typename T>
    uint
    animation::getIndexForTime(uint time, const std::vector<std::pair<uint,T>>& timetable)
    {
        const uint numKeys = timetable.size();
        if (numKeys == 0)
            return 0;

        uint id            = 0;
        uint lowerId    = 0;
        uint upperId    = numKeys;

        while(upperId - lowerId > 1)
        {
            id = (lowerId + upperId) >> 1;
            if (timetable[id].first > time)
                upperId = id;
            else
                lowerId = id;
        }

        assert(lowerId < numKeys);

        return lowerId;
    }
}



