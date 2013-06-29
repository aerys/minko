#include "DataProvider.hpp"

using namespace minko::scene::data;

void
DataProvider::unset(const std::string& propertyName)
{
	_values.erase(propertyName);
	_changedSignalSlots.erase(propertyName);

	if (_values.count(propertyName) == 0)
	{
		_names.erase(std::find(_names.begin(), _names.end(), propertyName));
		_propertyRemoved->execute(shared_from_this(), propertyName);
	}
}

void
DataProvider::registerProperty(const std::string& propertyName, std::shared_ptr<Value> value)
{
	bool isNewValue = _values.count(propertyName) == 0;

	_values[propertyName] = value;
	_changedSignalSlots[propertyName] = value->changed()->connect(std::bind(
		&Signal<ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));

	if (isNewValue)
	{
		_names.push_back(propertyName);
		_propertyAdded->execute(shared_from_this(), propertyName);
	}
	else
		_propertyChanged->execute(shared_from_this(), propertyName);
}


void
DataProvider::propertyWrapperInitHandler(const std::string& propertyName)
{
	_propertyAdded->execute(shared_from_this(), propertyName);

	/*_values[propertyName] = Value::create(std::bind(
		&Signal<ptr, const std::string&>::execute,
		_propertyChanged,
		shared_from_this(),
		propertyName
	));*/
}
