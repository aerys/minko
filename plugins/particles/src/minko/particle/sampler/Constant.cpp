#include "Constant.hpp"

using namespace minko::particle::sampler;

template <class T>
Constant<T>::Constant(T value)
	: _value (value)
{}

template <class T>
T
Constant<T>::value(float time) const
{
	return _value;
}

template <class T>
T*
Constant<T>::set(T* target, float time) const
{
	if (target == 0)
		target = new T();

	*target = _value;

	return target;
}


template <class T>
T
Constant<T>::max() const
{
	return _value;
}

template <class T>
T
Constant<T>::min() const
{
	return _value;
}