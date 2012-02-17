package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Dictionary;

	public final class DataBinding
	{
		private static const NO_KEY	: String		= "__no_key__";
		
		private var _bindings			: Dictionary				= new Dictionary(true);
		private var _values				: Object					= {};
		
		private var _propertyChanged	: Signal					= new Signal();
		
		public function get propertyChanged() : Signal
		{
			return _propertyChanged;
		}
		
		public function DataBinding()
		{
		}
		
		public function clone() : DataBinding
		{
			var clone 			: DataBinding 	= new DataBinding();
			var clonedBindings	: Dictionary	= clone._bindings;
			
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				var clonedTable		: Object	= {};
				
				clonedBindings[source] = clonedTable;
				
				for (var key : String in bindingTable)
					clonedTable[key] = bindingTable[key];
			}
			
			return clone;
		}
		
		public function add(bindable : IDataProvider) : DataBinding
		{
			var dataDescriptor 	: Object 	= bindable.dataDescriptor;
			
			for (var propertyName : String in dataDescriptor)
			{
				addProperty(
					propertyName,
					bindable,
					dataDescriptor[propertyName] as String
				);
			}
			
			return this;
		}
		
		public function remove(dataProvider : IDataProvider) : DataBinding
		{
			var dataDescriptor 	: Object 	= dataProvider.dataDescriptor;
			
			for (var parameterName : String in dataDescriptor)
				removeProperty(parameterName);
			
			return this;
		}
		
		public function setProperty(propertyName : String, value : Object) : DataBinding
		{
			var oldValue : Object = _values[propertyName];
			
			_values[propertyName] = value;
			
			_propertyChanged.execute(this, propertyName, oldValue, value);
			
			return this;
		}
		
		public function getProperty(propertyName : String) : Object
		{
			return _values[propertyName];
		}
		
		public function addProperty(propertyName 	: String,
									source			: IDataProvider,
									key				: Object	= null) : DataBinding
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
		
		public function removeProperty(propertyName : String) : DataBinding
		{
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
					(source as IDataProvider).changed.remove(
						propertyChangedHandler
					);
					
					delete _bindings[source];
				}
			}
			
			return this;
		}
		
		public function clear() : DataBinding
		{
			for (var source : Object in _bindings)
			{
				var bindingTable 	: Object 	= _bindings[source];
				
				for (var key : String in bindingTable)
					removeProperty(bindingTable[key]);
			}
			
			return this;
		}
		
		private function propertyChangedHandler(source : IDataProvider, key : Object) : void
		{
			key ||= NO_KEY;
			
			var bindingTable 	: Object = _bindings[source] as Object;
			var propertyName 	: String = bindingTable[key] as String;
			
			if (propertyName)
				setProperty(propertyName, key !== NO_KEY ? source[key] : source);
		}
	}
}