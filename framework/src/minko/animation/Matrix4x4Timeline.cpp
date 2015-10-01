/*
Copyright (c) 2014 Aerys

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
*/

#include "minko/animation/Matrix4x4Timeline.hpp"

#include "minko/data/Store.hpp"
#include "timeline_lookup.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::animation;

Matrix4x4Timeline::Matrix4x4Timeline(const std::string& 			propertyName,
									 uint 							duration,
									 const std::vector<uint>& 		timetable,
									 const std::vector<math::mat4>& matrices,
									 bool 							interpolate):
	AbstractTimeline(propertyName, duration),
	_matrices(),
	_interpolate(interpolate)
{
	initializeMatrixTimetable(timetable, matrices);
}

Matrix4x4Timeline::Matrix4x4Timeline(const Matrix4x4Timeline& matrix) :
    AbstractTimeline(matrix._propertyName, matrix._duration),
    _matrices(matrix._matrices.size()),
    _interpolate(matrix._interpolate)
{
    for (uint keyId = 0; keyId < matrix._matrices.size(); ++keyId)
    {
        _matrices[keyId] = matrix._matrices[keyId];
    }
}

AbstractTimeline::Ptr
Matrix4x4Timeline::clone()
{
    return Matrix4x4Timeline::create(*this);
}

void
Matrix4x4Timeline::initializeMatrixTimetable(const std::vector<uint>& 		timetable,
											 const std::vector<math::mat4>& matrices)
{
	if (timetable.empty())
		throw std::invalid_argument("timetable");
	if (matrices.empty())
		throw std::invalid_argument("matrices");
	if (timetable.size() != matrices.size())
		throw std::logic_error("The number of keys must match in both the 'timetable' and 'matrices' parameters.");

	const uint numKeys = timetable.size();

	_matrices.clear();
	_matrices.resize(numKeys);

	for (uint keyId = 0; keyId < numKeys; ++keyId)
	{
		_matrices[keyId].first	= timetable[keyId];
		_matrices[keyId].second	= matrices[keyId];
	}

	std::sort(
		_matrices.begin(),
		_matrices.end(),
		[](const std::pair<uint, math::mat4>& a, const std::pair<uint, math::mat4>& b)
		{
			return a.first < b.first;
		}
    );
}

void
Matrix4x4Timeline::update(uint time, 
						  data::Store& data, 
						  bool /*skipPropertyNameFormatting*/)
{
	if (_isLocked || _duration == 0 || _matrices.empty())
		return;

    if (_interpolate)
    	data.set(_propertyName, interpolate(time));
    else
    {
        const uint t = getTimeInRange(time, _duration + 1);
	    const uint keyId = getIndexForTime(t, _matrices);

    	data.set(_propertyName, _matrices[keyId].second);
    }
}

math::mat4
Matrix4x4Timeline::interpolate(uint time) const
{
    const auto t = getTimeInRange(time, _duration + 1);
	const auto keyId = getIndexForTime(t, _matrices);

    // all matrices are sorted in order of increasing time
    if (t < _matrices.front().first || t >= _matrices.back().first)
        return _matrices[keyId].second;

#ifdef DEBUG
	assert(keyId + 1 < (int)_matrices.size());
#endif
    
	const auto& current	= _matrices[keyId];
	const auto& next = _matrices[keyId + 1];
	const float ratio = current.first < next.first 
		? (t - current.first) / (float)(next.first - current.first)
		: 0.0f;

	return math::mix(current.second, next.second, ratio);
}