#pragma once

#include <iostream>
#include <sstream>



namespace std
{
    inline string to_string (int num)
    {
    ostringstream convert; // stream used for the conversion

    convert << num; // insert the textual representation of ‘Number’ in the characters    in the stream

    return convert.str();
    }
}
