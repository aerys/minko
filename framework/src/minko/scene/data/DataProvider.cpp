#include "DataProvider.hpp"

using namespace minko::scene::data;

void
DataProvider::unsetProperty(const std::string& propertyName)
{
	_values.erase(propertyName);

	if (_values.count(propertyName) == 0)
	{
		_names.erase(std::find(_names.begin(), _names.end(), propertyName));
		_propertyRemoved->execute(shared_from_this(), propertyName);
	}
}

void
DataProvider::propertyWrapperInitHandler(const std::string& propertyName)
{
	_propertyAdded->execute(shared_from_this(), propertyName);

	_values[propertyName] = DataProviderPropertyWrapper::create(std::bind(
		&Signal<ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));
}
