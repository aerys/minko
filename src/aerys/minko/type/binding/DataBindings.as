package aerys.minko.type.binding
{
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.DataProviderUsage;
	
	import flash.utils.Dictionary;

	public final class DataBindings
	{
		private var _owner							: ISceneNode				= null;
		
		private var _providers						: Vector.<IDataProvider> 	= new <IDataProvider>[];
		private var _bindingNames					: Vector.<String>			= new Vector.<String>();
		
		private var _bindingNameToValue				: Object					= {};
		private var _bindingNameToChangedSignal		: Object					= {};
		
		private var _bindingNameToProvider			: Object					= {};
		private var _providerToBindingNames			: Dictionary				= new Dictionary(); // dic[Vector.<String>[]]
		
		public function get owner() : ISceneNode
		{
			return _owner;
		}
		
		public function get numProviders() : uint
		{
			return _providers.length;
		}
		
		public function get numProperties() : uint
		{
			return _bindingNames.length;
		}
		
		public function DataBindings(owner : ISceneNode)
		{
			_owner = owner;
		}
		
		public function contains(dataProvider : IDataProvider) : Boolean
		{
			return _providers.indexOf(dataProvider) != -1;
		}
		
		public function addProvider(provider : IDataProvider) : void
		{
			if (_providerToBindingNames[provider])
				throw new Error('This provider is already bound.');
			
			var providerBindingNames	: Vector.<String>	= new <String>[];
			var dataDescriptor			: Object			= provider.dataDescriptor;
			
			provider.changed.add(providerChangedHandler);
			
			_providerToBindingNames[provider] = providerBindingNames;
			_providers.push(provider);
			
			for (var attrName : String in dataDescriptor)
			{
				// if this provider attribute is also a dataprovider, let's also bind it
				var bindingName	: String	= dataDescriptor[attrName];
				var attribute	: Object	= provider[attrName];
				
				if (_bindingNames.indexOf(bindingName) != -1)
					throw new Error(
						'Another data provider is already declaring the \'' + bindingName + '\' property.'
					);
				
				_bindingNameToProvider[bindingName] = provider;
				_bindingNameToValue[bindingName] = attribute;
				
				providerBindingNames.push(bindingName);
				_bindingNames.push(bindingName);
				
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, null, attribute);
			}
		}
		
		public function removeProvider(provider : IDataProvider) : void
		{
			var providerBindingsNames 	: Vector.<String> 	= _providerToBindingNames[provider];
			var tmpValues				: Object			= {};
			
			if (providerBindingsNames == null)
				throw new ArgumentError('Unkown provider.');
			
			var numBindings : uint = _bindingNames.length;
			for (var indexRead : uint = 0, indexWrite : uint = 0; indexRead < numBindings; ++indexRead)
			{
				var bindingName : String = _bindingNames[indexRead];
				
				if (providerBindingsNames.indexOf(bindingName) != -1)
				{
					tmpValues[bindingName] = _bindingNameToValue[bindingName];
					
					delete _bindingNameToValue[bindingName];
					delete _bindingNameToProvider[bindingName];
				}
				else
					_bindingNames[indexWrite++] = _bindingNames[indexRead];
			}
			_bindingNames.length = indexWrite;
			
			provider.changed.remove(providerChangedHandler);
			
			_providers.splice(_providers.indexOf(provider), 1);
			
			delete _providerToBindingNames[provider];
			
			for each (bindingName in providerBindingsNames)
			{
				var changedSignal : Signal = _bindingNameToChangedSignal[bindingName] as Signal;
				
				if (changedSignal != null)
				{
					changedSignal.execute(
						this, bindingName, tmpValues[bindingName], null
					);
				}
			}
		}
		
		public function removeAllProviders() : void
		{
			var numProviders : uint = this.numProviders;
			
			for (var providerId : int = numProviders - 1; providerId >= 0; --providerId)
				removeProvider(getProviderAt(providerId));
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
			_bindingNameToChangedSignal[bindingName] ||= new Signal(
				'DataBindings.changed[' + bindingName + ']'
			);
			
			Signal(_bindingNameToChangedSignal[bindingName]).add(callback);
		}
		
		public function removeCallback(bindingName	: String,
									   callback		: Function) : void
		{
			var signal : Signal = _bindingNameToChangedSignal[bindingName];
			if (!signal)
				throw new ArgumentError('Unkown property \'' + bindingName + '\'.');
			
			signal.remove(callback);
			
			if (signal.numCallbacks == 0)
				delete _bindingNameToChangedSignal[bindingName];
		}
		
		public function getProviderAt(index : uint) : IDataProvider
		{
			return _providers[index];
		}
		
		public function getProviderByBindingName(bindingName : String) : IDataProvider
		{
			if (_bindingNameToProvider[bindingName] == null)
				throw new ArgumentError('Unkown property \'' + bindingName + '\'.');
			
			return _bindingNameToProvider[bindingName];
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
		
		public function copySharedProvidersFrom(source : DataBindings) : void
		{
			var numProviders : uint = source._providers.length;
			
			for (var providerId : uint = 0; providerId < numProviders; ++providerId)
			{
				var provider : IDataProvider = source._providers[providerId];
				
				if (provider.usage == DataProviderUsage.SHARED)
					addProvider(provider);
			}
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
				var oldValue	: Object		= _bindingNameToValue[bindingName];
				var newValue	: Object		= source[attributeName];
				
				_bindingNameToValue[bindingName] = newValue;
				
				if (_bindingNameToChangedSignal[bindingName])
					_bindingNameToChangedSignal[bindingName].execute(this, bindingName, oldValue, newValue);
			}
		}
	}
}