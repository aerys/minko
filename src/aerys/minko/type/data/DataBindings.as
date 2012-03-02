package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Dictionary;

	public final class DataBindings
	{
		private static const NO_KEY	: String		= "__no_key__";
		
		private var _bindings			: Dictionary		= new Dictionary(true);
		private var _values				: Object			= {};
		private var _properties			: Vector.<String>	= new <String>[];
		
		private var _propertyChanged	: Object			= {};
		private var _propertyRemoved	: Object			= {};
		
		public function get numProperties() : uint
		{
			return _properties.length;
		}
		
		public function DataBindings()
		{
		}
		
		public function propertyExists(propertyName : String) : Boolean
		{
			return _values.hasOwnProperty(propertyName);
		}
		
		public function getPropertyChangedSignal(property : String) : Signal
		{
			var signal : Signal = _propertyChanged[property];
			
			if (!signal)
				_propertyChanged[property] = signal = new Signal();
			
			return signal;
		}
		
		public function getPropertyRemovedSignal(property : String) : Signal
		{
			var signal : Signal = _propertyRemoved[property];
			
			if (!signal)
				_propertyRemoved[property] = signal = new Signal();
			
			return signal;
		}
		
		public function clone() : DataBindings
		{
			var clone 			: DataBindings 	= new DataBindings();
			var clonedBindings	: Dictionary	= clone._bindings;
			
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				var clonedTable		: Object	= {};
				
				clonedBindings[source] = clonedTable;
				
				for (var key : String in bindingTable)
					clonedTable[key] = bindingTable[key];
			}
			
			for (var propertyName : String in _values)
				clone._values[propertyName] = _values[propertyName];
			
			clone._properties = clone._properties.slice();
			
			return clone;
		}
		
		public function add(dataProvider : IDataProvider) : DataBindings
		{
			var dataDescriptor 	: Object 	= dataProvider.dataDescriptor;
			
			dataProvider.changed.add(dataProviderChangedHandler);
			
			for (var propertyName : String in dataDescriptor)
			{
				addProperty(
					propertyName,
					dataProvider,
					dataDescriptor[propertyName] as String
				);
			}
			
			if (dataProvider is IDynamicDataProvider)
			{
				var dynamicProvider : IDynamicDataProvider = dataProvider
					as IDynamicDataProvider;
				
				dynamicProvider.propertyAdded.add(dynamicPropertyAddedHandler);
				dynamicProvider.propertyRemoved.add(dynamicPropertyRemovedHandler);
			}
						
			return this;
		}
		
		public function remove(dataProvider : IDataProvider) : DataBindings
		{
			var dataDescriptor 	: Object 	= dataProvider.dataDescriptor;
			
			for (var parameterName : String in dataDescriptor)
				removeProperty(parameterName);
			
			return this;
		}
		
		public function setProperty(propertyName : String, value : Object) : DataBindings
		{
			var oldValue : Object = _values[propertyName];
			
			if (!(_values.hasOwnProperty(propertyName)))
				_properties.push(propertyName);
			
			_values[propertyName] = value;
			
			getPropertyChangedSignal(propertyName).execute(
				this, propertyName, oldValue, value
			);
			
			return this;
		}
		
		public function setProperties(properties : Object) : DataBindings
		{
			for (var propertyName : String in properties)
				setProperty(propertyName, properties[propertyName]);
			
			return this;
		}
		
		public function getProperty(propertyName : String) : Object
		{
			return _values[propertyName];
		}
		
		public function getPropertyName(index : uint) : String
		{
			return _properties[index];
		}
		
		public function addProperty(propertyName 	: String,
									source			: IDataProvider,
									key				: Object	= null) : DataBindings
		{
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
		
		public function removeProperty(propertyName : String) : DataBindings
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
					
					dataProvider.changed.remove(
						propertyChangedHandler
					);
					
					if (dataProvider is IDynamicDataProvider)
					{
						var dynamicProvider : IDynamicDataProvider = dataProvider
							as IDynamicDataProvider;
						
						dynamicProvider.propertyAdded.remove(
							dynamicPropertyAddedHandler
						);
						dynamicProvider.propertyRemoved.remove(
							dynamicPropertyRemovedHandler
						);
					}
					
					delete _bindings[source];
				}
			}
			
			var numProperties	: int	= _properties.length - 1;
			
			_properties[_properties.indexOf(propertyName)] = _properties[numProperties];
			_properties.length = numProperties;
			
			var oldValue : Object = _values[propertyName];
			
			delete _values[propertyName];
			
			getPropertyRemovedSignal(propertyName).execute(
				this, propertyName
			);
			/*getPropertyChangedSignal(propertyName).execute(
				this, propertyName, oldValue, null
			);*/
			
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
		
		private function dataProviderChangedHandler(source : IDataProvider, key : Object) : void
		{
			var bindingTable 	: Object = _bindings[source] as Object;
			var propertyName 	: String = null;
			
			if (key)
			{
				// a single property has changed
				propertyName = bindingTable[key] as String;
				
				if (!propertyName)
					addProperty(source.dataDescriptor[key], source, key);
				else
					setProperty(propertyName, key !== NO_KEY ? source[key] : source);
			}
			else
			{
				// "some" properties have changed (ie. DataProvider.invalidate() was called)
				for (var key : Object in bindingTable)
				{
					propertyName = bindingTable[key];
					
					if (!propertyName)
						addProperty(source.dataDescriptor[key], source, key);
					else
						setProperty(propertyName, key !== NO_KEY ? source[key] : source);	
				}
			}
		}
		
		private function propertyChangedHandler(source : IDataProvider, key : Object) : void
		{
			key ||= NO_KEY;
			
			var bindingTable 	: Object = _bindings[source] as Object;
			var propertyName 	: String = bindingTable[key] as String;
			
			if (propertyName)
				setProperty(propertyName, key !== NO_KEY ? source[key] : source);
		}
		
		private function dynamicPropertyAddedHandler(source 	: IDynamicDataProvider,
													 property	: String) : void
		{
			addProperty(property, source, source.dataDescriptor[property]);
		}
		
		private function dynamicPropertyRemovedHandler(source 	: IDynamicDataProvider,
													   property	: String) : void
		{
			removeProperty(property);
		}
	}
}