#include "RandomValue.hpp"

using namespace minko::particle::sampler;

template <class T>
RandomValue<T>::RandomValue(T min, T max)
	: _min (min),
	  _delta(max - min)
{}

template <class T>
T
RandomValue<T>::value(float time) const
{
	return _min + _delta * rand01();
}

template <class T>
T*
RandomValue<T>::set(T* target, float time) const
{
	if (target == 0)
		target = new T();

	*target = _min + _delta * rand01();

	return target;
}

template <class T>
T
RandomValue<T>::max() const
{
	return _min + _delta;
}

template <class T>
T
RandomValue<T>::min() const
{
	return _min;
}