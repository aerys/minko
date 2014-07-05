/*
Copyright (c) 2013 Aerys

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

#include "minko/data/Container.hpp"

#include "minko/data/ArrayProvider.hpp"
#include "minko/data/Provider.hpp"
#include "minko/data/AbstractFilter.hpp"

using namespace minko;
using namespace minko::data;

uint Container::CONTAINER_ID = 0;

Container::Container() :
	std::enable_shared_from_this<Container>(),
	_providers(),
	_arrayLengths(data::Provider::create()),
	_propertyAdded(Container::PropertyChangedSignal::create()),
	_propertyRemoved(Container::PropertyChangedSignal::create()),
	_providerAdded(Signal<Ptr, Provider::Ptr>::create()),
	_providerRemoved(Signal<Ptr, Provider::Ptr>::create()),
	_containerId(CONTAINER_ID++)
{
}

void
Container::initialize()
{
	addProvider(_arrayLengths);
}

void
Container::addProvider(Provider::Ptr provider)
{
    // warning! each provider can be added mutiple times to the same container
    // example: the same material can be used by multiple meshes on the same node
    // assertProviderDoesNotExist(provider) does not apply here;

    // 
    if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
        _providersToNumUse[provider]++;
    else
    {
        _providersToNumUse[provider] = 1;
        _providers.push_back(provider);

        _propertySlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
            &Container::providerPropertyAddedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )));

        _propertySlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
            &Container::providerPropertyRemovedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )));

        _propertySlots[provider].push_back(provider->propertyChanged()->connect(std::bind(
            &Container::providerPropertyChangedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        )));

        for (auto property : provider->values())
            providerPropertyAddedHandler(provider, property.first);

        _providerAdded->execute(shared_from_this(), provider);
    }
}

void
Container::removeProvider(Provider::Ptr provider)
{
    assertProviderExists(provider);

    if (_providersToNumUse.find(provider) == _providersToNumUse.end())
        return;

    _providersToNumUse[provider]--;
    // we have to make sure the provider is not 
    if (_providersToNumUse[provider] == 0)
    {
        for (auto property : provider->values())
            providerPropertyRemovedHandler(provider, property.first);

        // erase all the slots (property added, changed, removed) for this provider
        _propertySlots.erase(provider);
        // destroy all signals that might have been created for each property declared by the provider
        // warning! erase the signal only if it has no callbacks anymore, otherwise it should be kept valid
        for (const auto& nameAndValue : provider->values())
            if (_propertyChanged.count(nameAndValue.first) != 0 && _propertyChanged[nameAndValue.first]->numCallbacks() == 0)
                _propertyChanged.erase(nameAndValue.first);

        _providers.erase(std::find(_providers.begin(), _providers.end(), provider));
        _providerToIndex.erase(provider);
        _providersToNumUse.erase(provider);

        _providerRemoved->execute(shared_from_this(), provider);
    }
}

/*
void
Container::addProvider(Provider::Ptr provider)
{
	//assertProviderDoesNotExist(provider);
	if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
		_providersToNumUse[provider]++;
	else
	{
		_providersToNumUse[provider] = 1;
		_providers.push_back(provider);

		_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyAdded()->connect(std::bind(
			&Container::providerPropertyAddedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		)));
	
		_propertyAddedOrRemovedSlots[provider].push_back(provider->propertyRemoved()->connect(std::bind(
			&Container::providerPropertyRemovedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		)));

        _providerValueChangedSlot[provider] = provider->providerValueChanged()->connect(std::bind(
            &Container::providerValueChangedHandler,
            shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2
        ));

		_providerReferenceChangedSlot[provider] = provider->propertyReferenceChanged()->connect(std::bind(
			&Container::providerReferenceChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

		for (auto property : provider->values())
			providerPropertyAddedHandler(provider, property.first);

		_providerAdded->execute(shared_from_this(), provider);
	}
}

void
Container::removeProvider(Provider::Ptr provider)
{
	assertProviderExists(provider);

	if (_providersToNumUse.find(provider) == _providersToNumUse.end())
		return;

	_providersToNumUse[provider]--;
	if (_providersToNumUse[provider] == 0)
	{
		for (auto property : provider->values())
			providerPropertyRemovedHandler(provider, property.first);
	
		_propertyAddedOrRemovedSlots.erase(provider);
		_providerValueChangedSlot.erase(provider);
		_providerReferenceChangedSlot.erase(provider);

		_providers.erase(std::find(_providers.begin(), _providers.end(), provider));
		_providerToIndex.erase(provider);
		_providersToNumUse.erase(provider);

		_providerRemoved->execute(shared_from_this(), provider);
	}
}
*/

void
Container::addProvider(std::shared_ptr<ArrayProvider> provider)
{
	//assertProviderDoesNotExist(provider);

	// Warning: the instruction order is very important here.
	if (std::find(_providers.begin(), _providers.end(), provider) != _providers.end())
		_providersToNumUse[provider]++;
	else
	{
		_providersToNumUse[provider] = 1;
		const auto	lengthPropertyName = provider->arrayName() + ".length";
		int			length = _arrayLengths->hasProperty(lengthPropertyName)
			? _arrayLengths->get<int>(lengthPropertyName)
			: 0;

		if (_providerToIndex.find(provider) == _providerToIndex.end())
			_providerToIndex[provider] = length;
		
		addProvider(std::dynamic_pointer_cast<Provider>(provider));
		provider->indexChanged()->execute(provider, length);

		_arrayLengths->set<int>(lengthPropertyName, ++length);
	}
}

void
Container::removeProvider(std::shared_ptr<ArrayProvider> provider)
{
	assertProviderExists(provider);

	if (_providersToNumUse[provider] - 1 == 0)
	{
		auto	index				= _providerToIndex[provider];
		auto	lengthPropertyName	= provider->arrayName() + ".length";
		int		length				= _arrayLengths->hasProperty(lengthPropertyName)
			? _arrayLengths->get<int>(lengthPropertyName)
			: 0;

		removeProvider(std::dynamic_pointer_cast<Provider>(provider));

#ifdef DEBUG
		if (index >= (uint)length)
			throw std::logic_error("ArrayProvider index is greater-equal than the array length");
#endif

        // if the provider was not the last one in the array, we cannot simply resize
        // we have to reorder all the providers of the same array to make sure it is kept
        // contiguous
		if (index != length - 1)
		{
			std::unordered_map<ProviderPtr, uint>& providerToIndex = _providerToIndex;

            // find the "last" provider of this array
			auto lastIt = std::find_if(
				_providers.begin(),
				_providers.end(),
				[&](Provider::Ptr p)
				{
					auto arrayProvider = std::dynamic_pointer_cast<ArrayProvider>(p);

					return arrayProvider && providerToIndex[p] == length - 1 &&
						arrayProvider->arrayName() == provider->arrayName();
					
				});
			auto last = std::dynamic_pointer_cast<ArrayProvider>(*lastIt);

            // swap the last provider of the array with the removed one
            // this way we can safely resize the provider array
			removeProvider(last);
			_providerToIndex[last] = index;
			addProvider(last);
			last->indexChanged()->execute(last, index);
		}

        // resize the provider array
		--length;
		if (length == 0)
            // if there are no providers left in the array, we unset the corresponding length property
			_arrayLengths->unset(lengthPropertyName);
		else
			_arrayLengths->set<int>(lengthPropertyName, length);
	}
}

bool
Container::hasProvider(std::shared_ptr<Provider> provider) const
{
	return std::find(_providers.begin(), _providers.end(), provider) != _providers.end();
}

bool
Container::hasProperty(const std::string& propertyName) const
{
	return _propertyNameToProvider.count(propertyName) != 0;
}

/*
Container::PropertyChangedSignalPtr
Container::propertyValueChanged(const std::string& propertyName)
{
	//assertPropertyExists(propertyName);

	if (_propValueChanged.count(propertyName) == 0)
	{
		_propValueChanged[propertyName] = Signal<Container::Ptr, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			Provider::Ptr provider = _propertyNameToProvider[propertyName];

			if (_providerValueChangedSlot.count(provider) == 0)
				_providerValueChangedSlot[provider] = provider->propertyValueChanged()->connect(std::bind(
					&Container::providerValueChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
		}
	}

	return _propValueChanged[propertyName];
}

Container::PropertyChangedSignalPtr
Container::propertyReferenceChanged(const std::string& propertyName)
{
	if (_propReferenceChanged.count(propertyName) == 0)
	{
		_propReferenceChanged[propertyName] = Signal<Container::Ptr, const std::string&>::create();

		if (_propertyNameToProvider.count(propertyName) != 0)
		{
			Provider::Ptr provider = _propertyNameToProvider[propertyName];

			if (_providerReferenceChangedSlot.count(provider) == 0)
			{
				_providerReferenceChangedSlot[provider] = provider->propertyReferenceChanged()->connect(std::bind(
					&Container::providerReferenceChangedHandler,
					shared_from_this(),
					std::placeholders::_1,
					std::placeholders::_2
				));
			}
		}
	}

	return _propReferenceChanged[propertyName];
}
*/

void
Container::assertPropertyExists(const std::string& propertyName) const
{
	if (!hasProperty(propertyName))
		throw std::invalid_argument(propertyName);	
}

void
Container::providerPropertyChangedHandler(ProviderPtr provider, const std::string& propertyName)
{
    auto formatedPropertyName = formatPropertyName(provider, propertyName);

    if (_propertyChanged.count(formatedPropertyName) != 0)
        propertyChanged(formatedPropertyName)->execute(shared_from_this(), formatedPropertyName);
}


/*
void
Container::providerValueChangedHandler(Provider::Ptr		provider,
									   const std::string& 	propertyName)
{
	auto formatedPropertyName = formatPropertyName(provider, propertyName);

	if (_propValueChanged.count(formatedPropertyName) != 0)
		propertyValueChanged(formatedPropertyName)->execute(shared_from_this(), formatedPropertyName);
}

void
Container::providerReferenceChangedHandler(Provider::Ptr		provider,
										   const std::string&	propertyName)
{
	auto formatedPropertyName = formatPropertyName(provider, propertyName);

	if (_propReferenceChanged.count(formatedPropertyName))
		propertyReferenceChanged(formatedPropertyName)->execute(shared_from_this(), formatedPropertyName);
}
*/

void
Container::providerPropertyAddedHandler(std::shared_ptr<Provider> 	provider,
                                        const std::string& 			propertyName)
{
    auto formatedPropertyName = formatPropertyName(provider, propertyName);

    if (_propertyNameToProvider.count(formatedPropertyName) != 0)
        throw std::logic_error("duplicate property name: " + formatedPropertyName);

    _propertyNameToProvider[formatedPropertyName] = provider;
    _propertyAdded->execute(shared_from_this(), formatedPropertyName);

    providerPropertyChangedHandler(provider, formatedPropertyName);
}


void
Container::providerPropertyRemovedHandler(std::shared_ptr<Provider> provider,
const std::string&		propertyName)
{

    auto formatedPropertyName = formatPropertyName(provider, propertyName);

    if (_propertyNameToProvider.count(formatedPropertyName) != 0)
    {
        _propertyNameToProvider.erase(formatedPropertyName);

        if (_propertyChanged.count(formatedPropertyName) && _propertyChanged[formatedPropertyName]->numCallbacks() == 0)
            _propertyChanged.erase(formatedPropertyName);

        _propertyRemoved->execute(shared_from_this(), formatedPropertyName);
    }
}

/*
void
Container::providerPropertyAddedHandler(std::shared_ptr<Provider> 	provider,
										const std::string& 			propertyName)
{	
	auto formatedPropertyName = formatPropertyName(provider, propertyName);

	if (_propertyNameToProvider.count(formatedPropertyName) != 0)
		throw std::logic_error("duplicate property name: " + formatedPropertyName);
	
	_propertyNameToProvider[formatedPropertyName] = provider;

	if (_propValueChanged.count(formatedPropertyName) != 0)
		_providerValueChangedSlot[provider] = provider->propertyValueChanged()->connect(std::bind(
			&Container::providerValueChangedHandler,
			shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		));

	_propertyAdded->execute(shared_from_this(), formatedPropertyName);

	providerValueChangedHandler(provider, formatedPropertyName);
}

void
Container::providerPropertyRemovedHandler(std::shared_ptr<Provider> provider,
										  const std::string&		propertyName)
{

	auto formatedPropertyName = formatPropertyName(provider, propertyName);

	if (_propertyNameToProvider.count(formatedPropertyName) != 0)
	{
		_propertyNameToProvider.erase(formatedPropertyName);
	
		if (_propValueChanged.count(formatedPropertyName) && _propValueChanged[formatedPropertyName]->numCallbacks() == 0)
			_propValueChanged.erase(formatedPropertyName);
	
		if (_propReferenceChanged.count(formatedPropertyName) && _propReferenceChanged[formatedPropertyName]->numCallbacks() == 0)
			_propReferenceChanged.erase(formatedPropertyName);
	
		_propertyRemoved->execute(shared_from_this(), formatedPropertyName);
	}
}
*/

std::string
Container::formatPropertyName(ProviderPtr provider, const std::string& propertyName) const
{

	auto arrayProvider = std::dynamic_pointer_cast<ArrayProvider>(provider);

	if (arrayProvider == nullptr)
		return propertyName;

#ifndef MINKO_NO_GLSL_STRUCT

	return arrayProvider->arrayName() + "[" + std::to_string(_providerToIndex.find(provider)->second) + "]." + propertyName;

#else

	return arrayProvider->arrayName() + NO_STRUCT_SEP + propertyName + "[" + std::to_string(_index) + "]";

#endif // MINKO_NO_GLSL_STRUCT
}

std::string
Container::unformatPropertyName(ProviderPtr provider, const std::string& formattedPropertyName) const
{

	auto arrayProvider = std::dynamic_pointer_cast<ArrayProvider>(provider);

	if (arrayProvider == nullptr)
		return formattedPropertyName;

	if (formattedPropertyName.substr(0, arrayProvider->arrayName().size()) != arrayProvider->arrayName())
		return unformatPropertyName(provider, formattedPropertyName);

#ifndef MINKO_NO_GLSL_STRUCT

	std::size_t pos = formattedPropertyName.rfind("].");

	if (pos == std::string::npos)
		return unformatPropertyName(provider, formattedPropertyName);

	return formattedPropertyName.substr(pos + 2);

#else

	std::size_t pos1 = formattedPropertyName.find_first_of(NO_STRUCT_SEP);
	std::size_t pos2 = formattedPropertyName.find_first_of('[');

	if (pos1 == std::string::npos || pos2 == std::string::npos)
		return unformatPropertyName(provider, formattedPropertyName);

	pos1 += NO_STRUCT_SEP.size();

	return formattedPropertyName.substr(pos1, pos2 - pos1);

#endif // MINKO_NO_GLSL_STRUCT
}

Container::Ptr
Container::filter(const std::set<data::AbstractFilter::Ptr>&	filters,
				  Container::Ptr								output) const
{
	if (output == nullptr)
		output = data::Container::create();

	for (auto& p : _providers)
	{
		if (p == _arrayLengths)
			continue;

		auto arrayProvider = std::dynamic_pointer_cast<ArrayProvider>(p);

		bool isProviderRelevant = true;
		for (auto& f : filters)
			if (!(*f)(p))
			{
				isProviderRelevant = false;
				break;
			}

		if (isProviderRelevant && output->hasProvider(p) == false)
		{
			if (arrayProvider)
				output->addProvider(arrayProvider);
			else
				output->addProvider(p);
		}
		else if (!isProviderRelevant && output->hasProvider(p))
		{
			if (arrayProvider)
				output->removeProvider(arrayProvider);
			else
				output->removeProvider(p);
		}
	}

	return output;
}

bool
Container::isLengthProperty(const std::string& propertyName) const
{
	auto foundProviderIt = _propertyNameToProvider.find(propertyName);

	return foundProviderIt == _propertyNameToProvider.end()
		? false
		: foundProviderIt->second.get() == _arrayLengths.get();
}