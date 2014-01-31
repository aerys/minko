package aerys.minko.type.binding
{
	import flash.utils.Dictionary;
	
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.DataProviderUsage;

	public final class DataBindings
	{
		private var _owner						: ISceneNode;
		
		private var _providers					: Vector.<IDataProvider>;
		private var _bindingNames				: Vector.<String>;
		
		private var _bindingNameToValue			: Object;
		private var _bindingNameToChangedSignal	: Object;
		
		private var _bindingNameToProvider		: Object;
		private var _providerToBindingNames		: Dictionary;
		
		private var _consumers					: Vector.<IDataBindingsConsumer>;
		
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
			
			_providers = new <IDataProvider>[];
			_bindingNames = new <String>[];
			_bindingNameToValue = {};
			_bindingNameToChangedSignal = {};
			_bindingNameToProvider = {};
			_providerToBindingNames = new Dictionary(true);
			_consumers = new <IDataBindingsConsumer>[];
		}
		
		public function contains(dataProvider : IDataProvider) : Boolean
		{
			return _providers.indexOf(dataProvider) != -1;
		}
		
		public function addProvider(provider : IDataProvider) : void
		{
			if (_providerToBindingNames[provider])
				throw new Error('This provider is already bound.');
			
			var dataDescriptor	: Object	= provider.dataDescriptor;
			
			provider.propertyChanged.add(propertyChangedHandler);
			if (provider is IDynamicDataProvider)
			{
				var dynamicProvider : IDynamicDataProvider = provider as IDynamicDataProvider;
				
				dynamicProvider.propertyAdded.add(addBinding);
				dynamicProvider.propertyRemoved.add(removeBinding);
			}
				
			_providerToBindingNames[provider] = new <String>[];
			_providers.push(provider);
			
			for (var propertyName : String in dataDescriptor)
				addBinding(
					provider,
					propertyName,
					dataDescriptor[propertyName],
					provider[propertyName]
				);
		}
		
		private function addBinding(provider		: IDataProvider,
									propertyName	: String,
									bindingName		: String,
									value			: Object) : void
		{
			var providerBindingNames	: Vector.<String> 	= _providerToBindingNames[provider];
			
			if (_bindingNames.indexOf(bindingName) != -1)
				throw new Error(
					'Another data provider is already declaring the \'' + bindingName
					+ '\' property.'
				);
			
			_bindingNameToProvider[bindingName] = provider;
			_bindingNameToValue[bindingName] = value;
			
			providerBindingNames.push(bindingName);
			_bindingNames.push(bindingName);
            
            var numConsumers : uint = _consumers.length;
            for (var consumerId : uint = 0; consumerId < numConsumers; ++consumerId)
                _consumers[consumerId].setProperty(bindingName, value);
			
			if (_bindingNameToChangedSignal[bindingName])
				_bindingNameToChangedSignal[bindingName].execute(this, bindingName, null, value);			
		}
		
		public function removeProvider(provider : IDataProvider) : void
		{
			var providerBindingsNames 	: Vector.<String> 	= _providerToBindingNames[provider];
			
			if (providerBindingsNames == null)
				throw new ArgumentError('Unknown provider.');
			
			var numProviders : uint = _providers.length - 1;
			
			provider.propertyChanged.remove(propertyChangedHandler);
			if (provider is IDynamicDataProvider)
			{
				var dynamicProvider : IDynamicDataProvider = provider as IDynamicDataProvider;
				
				dynamicProvider.propertyAdded.remove(addBinding);
				dynamicProvider.propertyRemoved.remove(removeBinding);
			}
			
			_providers[_providers.indexOf(provider)] = _providers[numProviders];
			_providers.length = numProviders;
			delete _providerToBindingNames[provider];
			
			var dataDescriptor : Object = provider.dataDescriptor;
			
			for (var propertyName : String in dataDescriptor)
				removeBinding(
					provider,
					propertyName,
					dataDescriptor[propertyName],
					provider[propertyName]
				);
		}
		
		public function removeBinding(provider		: IDataProvider,
									  propertyName	: String,
									  bindingName	: String,
									  value			: Object) : void
		{
			var numBindings 	: uint 		= _bindingNames.length - 1;
			var changedSignal 	: Signal 	= _bindingNameToChangedSignal[bindingName] as Signal;

			delete _bindingNameToValue[bindingName];
			delete _bindingNameToProvider[bindingName];
			
			_bindingNames[_bindingNames.indexOf(bindingName)] = _bindingNames[numBindings];
			_bindingNames.length = numBindings;
            
            var numConsumers : uint = _consumers.length;
            for (var consumerId : uint = 0; consumerId < numConsumers; ++consumerId)
                _consumers[consumerId].setProperty(bindingName, null);
				
			if (changedSignal != null)
				changedSignal.execute(this, bindingName, value, null);
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
			_bindingNameToChangedSignal[bindingName] ||= new Signal(bindingName);
			
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
			if (_bindingNames.indexOf(bindingName) < 0)
				throw new Error('The property \'' + bindingName + '\' does not exist.');
			
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
        
        public function hasProvider(provider : IDataProvider) : Boolean
        {
			for each (var accessibleProvider : IDataProvider in _providers)
			{
				if (accessibleProvider == provider)
					return true;
			}
			return false;
		}
		
		private function propertyChangedHandler(source 			: IDataProvider,
												propertyName 	: String,
												bindingName		: String,
												value			: Object) : void
		{
			if (propertyName == null)
				throw new Error('DataProviders must change only one property at a time.');

			var oldValue : Object = _bindingNameToValue[bindingName];
			
			_bindingNameToValue[bindingName] = value;
			
			var numConsumers : uint = _consumers.length;
			for (var consumerId : uint = 0; consumerId < numConsumers; ++consumerId)
				_consumers[consumerId].setProperty(bindingName, value);
			
			if (_bindingNameToChangedSignal[bindingName])
				_bindingNameToChangedSignal[bindingName].execute(
					this,
					bindingName,
					oldValue,
					value
				);
		}
		
		public function addConsumer(consumer : IDataBindingsConsumer) : void
		{
			_consumers.push(consumer);
			
			var numProperties : uint = this.numProperties;
			for (var propertyId : uint = 0; propertyId < numProperties; ++propertyId)
			{
				var bindingName : String = _bindingNames[propertyId];
				
				consumer.setProperty(bindingName, _bindingNameToValue[bindingName]);
			}
		}
		
		public function removeConsumer(consumer : IDataBindingsConsumer) : void
		{
			var numConsumers : uint = _consumers.length - 1;
			var index : int = _consumers.indexOf(consumer);
			
			if (index < 0)
				throw new Error('This consumer does not exist.');
			
			_consumers[index] = _consumers[numConsumers];
			_consumers.length = numConsumers;
		}
	}
}