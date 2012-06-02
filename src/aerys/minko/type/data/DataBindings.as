package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Dictionary;

	public class DataBindings
	{
		private var _numProviders					: uint 						= 0;
		private var _providers						: Vector.<IDataProvider> 	= new <IDataProvider>[];
		private var _bindingNames					: Vector.<String>			= new Vector.<String>();
		
		private var _bindingNameToValue				: Object					= {};
		private var _bindingNameToChangedSignal		: Object					= {};
		
		private var _providerToBindingNames			: Dictionary				= new Dictionary(); // dic[Vector.<String>[]]
		private var _attributeToProviders			: Dictionary				= new Dictionary(); // dic[Vector.<IDataProvider>[]]
		private var _attributeToProvidersAttrNames	: Dictionary				= new Dictionary(); // dic[Vector.<String>[]]
		
		public function get numProviders() : uint
		{
			return _numProviders;
		}
		
		public function get numProperties() : uint
		{
			return _bindingNames.length;
		}
		
		public function DataBindings()
		{
		}
		
		public function addProvider(provider : IDataProvider) : void
		{
			if (_providerToBindingNames[provider])
				throw new Error('This provider is already bound.');
			
			var providerBindingNames	: Vector.<String>	= new Vector.<String>();
			var dataDescriptor			: Object			= provider.dataDescriptor;
			
			provider.changed.add(providerChangedHandler);
			
			for (var attrName : String in dataDescriptor)
			{
				// if this provider attribute is also a dataprovider, let's also bind it
				var bindingName	: String		= dataDescriptor[attrName];
				var attribute	: Object		= provider[attrName]
				var dpAttribute	: IDataProvider	= attribute as IDataProvider;
				
				if (_bindingNames.indexOf(bindingName) != -1)
					throw new Error('Another Dataprovider is already declaring "' + bindingName + '".');
				
				if (dpAttribute != null)
				{
					dpAttribute.changed.add(providerPropertyChangedHandler);
					
					_attributeToProviders[dpAttribute]			||= new <IDataProvider>[];
					_attributeToProvidersAttrNames[dpAttribute]	||= new <String>[];
					
					_attributeToProviders[dpAttribute].push(provider);
					_attributeToProvidersAttrNames[dpAttribute].push(attrName);
				}
				
				_bindingNameToValue[bindingName] = attribute;
				
				providerBindingNames.push(bindingName);
				_bindingNames.push(bindingName);
				
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, attribute);
			}
			
			_providerToBindingNames[provider] = providerBindingNames;
			_providers.push(provider);
			++_numProviders;
		}
		
		public function removeProvider(provider : IDataProvider) : void
		{
			var bindingNames : Vector.<String> = _providerToBindingNames[provider];
			
			if (bindingNames == null)
				throw new ArgumentError('Unkown provider.');
			
			for each (var bindingName : String in bindingNames)
			{
				var indexOf : int = _bindingNames.indexOf(bindingName);
				
				_bindingNames.splice(indexOf, 1);
				
				if (_bindingNameToValue[bindingName] is IDataProvider)
					IDataProvider(_bindingNameToValue[bindingName]).changed.remove(providerPropertyChangedHandler);
				
				delete _bindingNameToValue[bindingName];
			}
			
			var attributesToDelete : Vector.<Object> = new Vector.<Object>();
			
			for (var attribute : Object in _attributeToProviders)
			{
				var providers		: Vector.<IDataProvider>	= _attributeToProviders[attribute];
				var attrNames		: Vector.<String>			= _attributeToProvidersAttrNames[attribute];
				var indexOfProvider	: int						= providers.indexOf(provider);
				
				if (indexOfProvider != -1)
				{
					providers.splice(indexOfProvider, 1);
					attrNames.splice(indexOfProvider, 1);
				}
				
				if (providers.length == 0)
					attributesToDelete.push(attribute);
			}
			
			for (var attributeToDelete : Object in attributesToDelete)
			{
				delete _attributeToProviders[attributeToDelete];
				delete _attributeToProvidersAttrNames[attributeToDelete];
			}
			
			provider.changed.remove(providerChangedHandler);
			
			--_numProviders;
			_providers.splice(_providers.indexOf(provider), 1);
			
			delete _providerToBindingNames[provider];
			
			for each (bindingName in bindingNames)
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, null);
		}
		
		public function hasCallback(bindingName	: String,
									callback	: Function) : Boolean
		{
			var signal : Signal = _bindingNameToChangedSignal[bindingName];
			
			return signal != null && signal.hasCallback(callback);
		}
		
		public function addCallback(bindingName : String,
									callback	: Function) : void
		{
			_bindingNameToChangedSignal[bindingName] ||=
				new Signal('DataBindings.changed[' + bindingName + ']');
			
			Signal(_bindingNameToChangedSignal[bindingName]).add(callback);
		}
		
		public function removeCallback(bindingName	: String,
									   callback		: Function) : void
		{
			var signal : Signal = _bindingNameToChangedSignal[bindingName];
			if (!signal)
				throw new Error('Unkown property \'' + bindingName + '\'.');
			
			signal.remove(callback);
			
			if (signal.numCallbacks == 0)
				delete _bindingNameToChangedSignal[bindingName];
		}
		
		public function getProviderAt(index : uint) : IDataProvider
		{
			return _providers[index];
		}
		
		public function propertyExists(bindingName : String) : Boolean
		{
			return _bindingNameToValue.hasOwnProperty(bindingName);
		}

		public function getProperty(bindingName : String) : *
		{
			return _bindingNameToValue[bindingName];
		}
		
		public function getPropertyName(bindingIndex : uint) : String
		{
			if (bindingIndex > numProperties)
				throw new ArgumentError('No such binding');
			
			return _bindingNames[bindingIndex];
		}
		
		private function providerChangedHandler(source : IDataProvider, attributeName : String) : void
		{
			if (attributeName == null)
			{
				throw new Error('DataProviders must change one property at a time.');
			}
			else if (attributeName == 'dataDescriptor')
			{
				removeProvider(source);
				addProvider(source);
			}
			else
			{
				var bindingName : String		= source.dataDescriptor[attributeName];
				var oldDpValue	: IDataProvider	= _bindingNameToValue[bindingName] as IDataProvider;
				var newValue	: Object		= source[attributeName];
				var newDpValue	: IDataProvider	= newValue as IDataProvider;
				
				// we are replacing a data provider. We must remove listeners and related mapping keys
				if (oldDpValue != null)
				{
					oldDpValue.changed.remove(providerPropertyChangedHandler);
					
					var providers	: Vector.<IDataProvider>	= _attributeToProviders[oldDpValue];
					var attrNames	: Vector.<String>			= _attributeToProvidersAttrNames[oldDpValue];
					
					if (providers.length == 1)
					{
						delete _attributeToProviders[oldDpValue];
						delete _attributeToProvidersAttrNames[oldDpValue];
					}
					else
					{
						var index : uint = providers.indexOf(source);
						providers.splice(index, 1);
						attrNames.splice(index, 1);
					}
				}
				
				// the new value for this key is a dataprovider, we must listen changes.
				if (newDpValue != null)
				{
					newDpValue.changed.add(providerPropertyChangedHandler);
					
					_attributeToProviders[newDpValue]			||= new <IDataProvider>[];
					_attributeToProvidersAttrNames[newDpValue]	||= new <String>[];
					
					_attributeToProviders[newDpValue].push(source);
					_attributeToProvidersAttrNames[newDpValue].push(attributeName);
				}
				
				_bindingNameToValue[bindingName] = newValue;
				
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, newValue);
			}
		}
		
		private function providerPropertyChangedHandler(source : IDataProvider, key : String) : void
		{
			var providers		: Vector.<IDataProvider>	= _attributeToProviders[source];
			var attrNames		: Vector.<String>			= _attributeToProvidersAttrNames[source];
			var numProviders	: uint						= providers.length;
			
			for (var providerId : uint = 0; providerId < numProviders; ++providerId)
			{
				var provider	: IDataProvider	= providers[providerId];
				var attrName	: String		= attrNames[providerId];
				var bindingName : String		= provider.dataDescriptor[attrName];
				
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, source);
			}
		}
	}
}