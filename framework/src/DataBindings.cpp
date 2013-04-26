#include "DataBindings.hpp"

DataBindings::DataBindings() :
	enable_shared_from_this()
{

}

void
DataBindings::addProvider(std::shared_ptr<DataProvider> provider)
{
	std::list<std::shared_ptr<DataProvider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it != _providers.end())
		throw std::invalid_argument("provider");

	_providers.push_back(provider);

	for (auto property : provider->values())
	{
		if (_propertyNameToProvider.count(property.first) != 0)
			throw std::logic_error("Duplicate binding property name: " + property.first);

		_propertyNameToProvider[property.first] = provider;

		if (_propertyChanged.count(property.first) != 0)
			_dataProviderPropertyChangedCd[provider] = (*provider->propertyChanged()) += std::bind(
				&DataBindings::dataProviderPropertyChangedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
			);
	}
}

void
DataBindings::removeProvider(std::shared_ptr<DataProvider> provider)
{
	std::list<std::shared_ptr<DataProvider>>::iterator it = std::find(
		_providers.begin(), _providers.end(), provider
	);

	if (it == _providers.end())
		throw std::invalid_argument("provider");

	_providers.erase(it);

	for (auto property : provider->values())
		_propertyNameToProvider.erase(property.first);

	if (_dataProviderPropertyChangedCd.count(provider) != 0)
		(*provider->propertyChanged()) -= _dataProviderPropertyChangedCd[provider];

	_dataProviderPropertyChangedCd.erase(provider);
}

bool
DataBindings::hasProvider(std::shared_ptr<DataProvider> provider)
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
DataBindings::hasProperty(const std::string& propertyName)
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

DataBindings::PropertyChangedSignal
DataBindings::propertyChangedSignal(const std::string& propertyName)
{
	//assertPropertyExists(propertyName);

	if (_propertyChanged.count(propertyName) == 0)
	{
		_propertyChanged[propertyName] = Signal<std::shared_ptr<DataBindings>, std::shared_ptr<DataProvider>, const std::string&>::create();

		std::shared_ptr<DataProvider> provider = _propertyNameToProvider[propertyName];

		if (_dataProviderPropertyChangedCd.count(provider) == 0)
			_dataProviderPropertyChangedCd[provider] = (*provider->propertyChanged()) += std::bind(
				&DataBindings::dataProviderPropertyChangedHandler, shared_from_this(), std::placeholders::_1, std::placeholders::_2
			);
	}

	return _propertyChanged[propertyName];
}

void
DataBindings::assertPropertyExists(const std::string& propertyName)
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
DataBindings::dataProviderPropertyChangedHandler(std::shared_ptr<DataProvider> provider, const std::string& propertyName)
{
	Minko::log("dataProviderPropertyChangedHandler: " + propertyName);

	if (_propertyChanged.count(propertyName) != 0)
		(*propertyChangedSignal(propertyName))(shared_from_this(), provider, propertyName);
}