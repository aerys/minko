package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Dictionary;

	public class DataBindings
	{
		private var _bindingNames					: Vector.<String>	= new Vector.<String>();
		
		private var _bindingNameToValue				: Object			= {};
		private var _bindingNameToChangedSignal		: Object			= {};
		
		private var _providerToBindingNames			: Dictionary		= new Dictionary(); // dic[Vector.<String>[]]
		private var _attributeToProviders			: Dictionary		= new Dictionary(); // dic[Vector.<IDataProvider>[]]
		private var _attributeToProvidersAttrNames	: Dictionary		= new Dictionary(); // dic[Vector.<String>[]]
		
		public function get numProperties() : uint
		{
			return _bindingNames.length;
		}
		
		public function DataBindings()
		{
		}
		
		public function add(provider : IDataProvider) : void
		{
			if (_providerToBindingNames[provider])
				throw new Error('This provider is already binded');
			
			var providerBindingNames	: Vector.<String>	= new Vector.<String>();
			var dataDescriptor			: Object			= provider.dataDescriptor;
			
			provider.changed.add(onProviderChange);
			
			for (var attrName : String in dataDescriptor)
			{
				var bindingName	: String		= dataDescriptor[attrName];
				
				if (_bindingNames.indexOf(bindingName) != -1)
					throw new Error('A Dataprovider is already defining this binding. Cannot overwrite.');
				
				// if this provider attribute is also a dataprovider, let's also bind it
				var attribute	: Object		= provider[attrName]
				var dpAttribute	: IDataProvider	= attribute as IDataProvider;
				
				if (dpAttribute != null)
				{
					dpAttribute.changed.add(onProviderAttributeChange);
					
					if (!_attributeToProviders[dpAttribute])
						_attributeToProviders[dpAttribute] = new <IDataProvider>[];
					
					if (!_attributeToProvidersAttrNames[dpAttribute])
						_attributeToProvidersAttrNames[dpAttribute] = new <String>[];
					
					_attributeToProviders[dpAttribute].push(provider);
					_attributeToProvidersAttrNames[dpAttribute].push(attrName);
				}
				
				_bindingNameToValue[bindingName] = attribute;
				
				providerBindingNames.push(bindingName);
				_bindingNames.push(bindingName);
			}
			
			_providerToBindingNames[provider] = providerBindingNames;
		}
		
		public function remove(dataProvider : IDataProvider) : void
		{
			var bindingNames : Vector.<String> = _providerToBindingNames[dataProvider];
			
			if (bindingNames == null)
				throw new ArgumentError('No such provider was binded');
			
			dataProvider.changed.remove(onProviderChange);
			
			for (var bindingName : String in bindingNames)
			{
				_bindingNames.splice(_bindingNames.indexOf(bindingName), 1);
				
				if (_bindingNameToValue[bindingName] is IDataProvider)
					IDataProvider(_bindingNameToValue[bindingName]).changed.remove(onProviderAttributeChange);
				
				delete _bindingNameToValue[bindingName];
				
			}
			
			var attributesToDelete : Vector.<Object> = new Vector.<Object>();
			
			for (var attribute : Object in _attributeToProviders)
			{
				var providers		: Vector.<IDataProvider>	= _attributeToProviders[attribute];
				var attrNames		: Vector.<String>			= _attributeToProvidersAttrNames[attribute];
				var indexOfProvider	: int						= providers.indexOf(dataProvider);
				
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
		}
		
		public function getPropertyChangedSignal(bindingName : String) : Signal
		{
			if (!_bindingNameToChangedSignal[bindingName])
				_bindingNameToChangedSignal[bindingName] = new Signal('DataBindings.changed[' + bindingName + ']');
			
			return _bindingNameToChangedSignal[bindingName];
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
		
		/**
		 * A provider attribute has changed, and the provider tells us.
		 * For example, camera.fov has changed, the camera dispatches a 'changed' signal with 'fov' as attributeName.
		 */		
		private function onProviderChange(source : IDataProvider, attributeName : String) : void
		{
			if (attributeName == 'dataDescriptor')
			{
				throw new Error('implement');
			}
			else if (attributeName == null)
			{
				throw new Error('implement');
			}
			else
			{
				var bindingName : String		= source.dataDescriptor[attributeName];
				var oldDPValue	: Object		= _bindingNameToValue[bindingName];
				var newDPValue	: IDataProvider	= source[attributeName] as IDataProvider;
				
				if ()
				{
					
					
					_bindingNameToValue[bindingName] = newValue;
					
				}
				
				
				getPropertyChangedSignal(bindingName).execute(this, bindingName, newValue);
			}
		}
		
		/**
		 * A provider attribute has changed, and the attribute tells us.
		 * For example, camera.localToWorld has been updated.
		 */
		private function onProviderAttributeChange(source : IDataProvider, key : String) : void
		{
			var providers		: Vector.<IDataProvider>	= _attributeToProviders[source];
			var attrNames		: Vector.<String>			= _attributeToProvidersAttrNames[source];
			var numProviders	: uint						= providers.length;
			
			// can do faster than that :p
			for (var providerId : uint = 0; providerId < numProviders; ++providerId)
				onProviderChange(providers[providerId], attrNames[providerId]);
		}
	}
}