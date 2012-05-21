package aerys.minko.type.data
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.type.Signal;
	
	import flash.utils.Dictionary;

	public final class DataBindings
	{
		use namespace minko_render;
		
		private static const NO_KEY	: String		= "__no_key__";
		
		private var _bindings			: Dictionary		= new Dictionary(true);
		private var _values				: Object			= new Object();
		private var _properties			: Vector.<String>	= new <String>[];
		private var _propertyToProvider	: Object			= {};
		
		private var _propertyChanged	: Object			= {};
		
		public function get numProperties() : uint
		{
			return _properties.length;
		}
		
		public function propertyExists(propertyName : String) : Boolean
		{
			return _values.hasOwnProperty(propertyName);
		}
		
		public function add(dataProvider : IDataProvider) : DataBindings
		{
			if (_bindings[dataProvider])
				throw new Error('This data provider is already bound.');
			
			var dataDescriptor 	: Object 	= dataProvider.dataDescriptor;
			
			dataProvider.changed.add(dataProviderChangedHandler);
			
			for (var propertyName : String in dataDescriptor)
			{
				var key 		: String		= dataDescriptor[propertyName] as String;
				var property 	: IDataProvider = dataProvider[key] as IDataProvider;
				
				if (property != null)
					addProperty(propertyName, property, null);
				
				addProperty(propertyName, dataProvider, key);
			}
			
			return this;
		}
		
		public function remove(dataProvider : IDataProvider) : DataBindings
		{
			if (!dataProvider)
				throw new Error("The argument 'dataProvider' cannot be null.");
			
			var bindingTable	: Object	= _bindings[dataProvider];
			
			for (var key : String in bindingTable)
				deleteProperty(bindingTable[key]);
			
			delete _bindings[dataProvider];
			
			if (dataProvider.changed.hasCallback(propertyChangedHandler))
				dataProvider.changed.remove(propertyChangedHandler);
			
			dataProvider.changed.remove(dataProviderChangedHandler);
			
			return this;
		}
		
		public function getDataProvider(propertyName : String) : IDataProvider
		{
			var provider : IDataProvider = _propertyToProvider[propertyName] as IDataProvider;
			
			if (!provider)
				throw new Error('The property \'' + propertyName + '\' has no data provider.');
			
			return provider;
		}
		
		public function getProperty(propertyName : String) : Object
		{
			return _values[propertyName];
		}
		
		public function getPropertyName(index : uint) : String
		{
			return _properties[index];
		}
		
		private function setProperty(propertyName	: String,
									 newValue		: Object) : DataBindings
		{
			if (newValue === null)
				throw new Error("The argument 'newValue' cannot be null.");
			
			if (_properties.indexOf(propertyName) < 0)
				_properties.push(propertyName);
			
			_values[propertyName] = newValue;
			
			signalChange(propertyName, newValue);
			
			return this;
		}
		
		private function setProperties(properties : Object) : DataBindings
		{
			for (var propertyName : String in properties)
				setProperty(propertyName, properties[propertyName]);
			
			return this;
		}
		
		private function addProperty(propertyName 	: String,
									source			: IDataProvider,
									key				: Object	= null) : DataBindings
		{
			if (_propertyToProvider[propertyName])
				removeProperty(propertyName);
			
			var bindingTable : Object = _bindings[source] as Object;
			
			if (!bindingTable)
			{
				_bindings[source] = bindingTable = {};
				source.changed.add(propertyChangedHandler);
			}
			
			if (key === null)
				key = NO_KEY;
			
			bindingTable[key] = propertyName;
			
			setProperty(propertyName, key !== NO_KEY ? source[key] : source);
			
			return this;
		}
		
		private function removeProperty(propertyName : String) : DataBindings
		{
			var numSources	: int	= 0;
			
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				var numKeys		 	: int		= 0;
				var numDeletedKeys	: int		= 0;
				
				for (var key : String in bindingTable)
				{
					++numKeys;
					
					if (bindingTable[key] == propertyName)
					{
						++numDeletedKeys;
						delete bindingTable[key];
					}
				}
				
				if (numKeys == numDeletedKeys)
				{
					var dataProvider : IDataProvider = source as IDataProvider;
					
					dataProvider.changed.remove(propertyChangedHandler);
					
					delete _bindings[source];
				}
			}
			
			deleteProperty(propertyName);
			
			return this;
		}
		
		public function clear() : DataBindings
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				
				for (var key : String in bindingTable)
					removeProperty(bindingTable[key]);
			}
			
			return this;
		}
		
		public function clone(exclude : Vector.<String> = null) : DataBindings
		{
			var clone 			: DataBindings 	= new DataBindings();
			var clonedBindings	: Dictionary	= clone._bindings;
			
			for (var source : Object in _bindings)
			{
				var dataProvider	: IDataProvider	= source as IDataProvider;
				var bindingTable 	: Object 		= _bindings[source];
				
				for (var key : String in bindingTable)
					if (exclude != null && exclude.indexOf(bindingTable[key]) < 0)
						clone.addProperty(bindingTable[key], dataProvider, key);
			}
			
			return clone;
		}
		
		public function getPropertyChangedSignal(property : String) : Signal
		{
			var signal : Signal = _propertyChanged[property];
			
			if (!signal)
				_propertyChanged[property] = signal = new Signal('DataBindings[' + property + '].changed');
			
			return signal;
		}
		
		private function deleteProperty(propertyName : String) : void
		{
			var numProperties	: uint		= _properties.length - 1;
			var index			: int		= _properties.indexOf(propertyName);
			
			_properties[index] = _properties[numProperties];
			_properties.length = numProperties;
			
			delete _values[propertyName];
			delete _propertyToProvider[propertyName];
//			delete _propertyChanged[propertyName];
			
			signalChange(propertyName, null);
		}
		
		private function propertyChangedHandler(source : IDataProvider, key : Object) : void
		{
			if (key == 'dataDescriptor')
				return;

			key ||= NO_KEY;
			
			var bindingTable 	: Object = _bindings[source] as Object;
			var propertyName 	: String = bindingTable[key] as String;
			
			if (propertyName)
				setProperty(propertyName, key !== NO_KEY ? source[key] : source);
		}
		
		private function signalChange(propertyName 	: String,
									  newValue		: Object) : void
		{
			getPropertyChangedSignal(propertyName).execute(this, propertyName, newValue);
		}
		
		private function dataProviderChangedHandler(dataProvider	: IDataProvider,
													property		: String) : void
		{
			if (property == "dataDescriptor")
			{
				remove(dataProvider);
				add(dataProvider);
			}
		}
	}
}