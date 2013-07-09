#include "LinearlyInterpolatedValue.hpp"

using namespace minko::particle::sampler;

template <class T>
LinearlyInterpolatedValue<T>::LinearlyInterpolatedValue(T start, T end)
	: _start (start),
	  _delta (end - start)
{}

template <class T>
T
LinearlyInterpolatedValue<T>::value(float time) const
{
	return _start + _delta * time;
}

template <class T>
T*
LinearlyInterpolatedValue<T>::set(T* target, float time) const
{
	if (target == 0)
		target = new T();

	*target = _start + _delta * time;

	return target;
}

template <class T>
T
LinearlyInterpolatedValue<T>::max() const
{
	return _start + _delta;
}

template <class T>
T
LinearlyInterpolatedValue<T>::min() const
{
	return _start;
}
