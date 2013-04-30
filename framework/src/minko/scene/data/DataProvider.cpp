#include "DataProvider.hpp"

using namespace minko::scene::data;

void
DataProvider::unsetProperty(const std::string& propertyName)
{
	_values.erase(propertyName);

	if (_values.count(propertyName) == 0)
	{
		_names.erase(std::find(_names.begin(), _names.end(), propertyName));
		(*_propertyRemoved)(shared_from_this(), propertyName);
	}
}
