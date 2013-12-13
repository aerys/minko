package aerys.minko.type.binding
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.DataProviderUsage;
	
	import flash.utils.Dictionary;
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public dynamic class DataProvider extends Proxy implements IDynamicDataProvider
	{	
		private var _usage				: uint;
		private var _name				: String;
		private var _descriptor			: Object;
		
		private var _propertyChanged	: Signal;
		private var _propertyAdded		: Signal;
		private var _propertyRemoved	: Signal;
			
		private var _nameToProperty		: Object;
		private var _propertyToNames	: Dictionary;
		
		public function get usage() : uint
		{
			return _usage;
		}
		
		public function get dataDescriptor() : Object
		{
			return _descriptor;
		}
		
		public function get propertyChanged() : Signal
		{
			return _propertyChanged;
		}
		
		public function get propertyAdded() : Signal
		{
			return _propertyAdded;
		}
		
		public function get propertyRemoved() : Signal
		{
			return _propertyRemoved;
		}
		
		public function get name() : String
		{
			return _name;
		}
		
		public function set name(v : String) : void
		{
			_name = v;
		}
		
		public function DataProvider(properties	: Object	= null, 
									 name		: String	= null,
									 usage		: uint		= 1)
		{
			_name	= name;
			_usage	= usage;
			
			initialize(properties);
		}
		
		private function initialize(properties : Object) : void
		{
			_descriptor = {};
			_propertyChanged = new Signal('DataProvider.changed');
			_propertyAdded = new Signal('DataProvider.propertyAdded');
			_propertyRemoved = new Signal('DataProvider.propertyRemoved');
			_nameToProperty = {};
			_propertyToNames = new Dictionary();
						
			setProperties(properties);
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			return getProperty(String(name));
		}
		
		override flash_proxy function setProperty(name : *, value : *) : void
		{
			setProperty(String(name), value);
		}
		
		override flash_proxy function deleteProperty(name : *) : Boolean
		{
			removeProperty(String(name));
			
			return true;
		}
		
		public function getProperty(name : String) : *
		{
			if (_nameToProperty)
				return _nameToProperty[name];
			else
				return null;
		}
		
		public function setProperty(name : String, newValue : Object) : DataProvider
		{
			var oldValue 			: Object			= _nameToProperty[name];
			var oldWatchedValue	    : IWatchable		= oldValue as IWatchable;
			var newWatchedValue	    : IWatchable		= newValue as IWatchable;
			var oldPropertyNames	: Vector.<String>	= _propertyToNames[oldWatchedValue];
			var newPropertyNames	: Vector.<String>	= _propertyToNames[newWatchedValue];
			
            if (newWatchedValue != oldWatchedValue)
            {
    			if (oldWatchedValue != null)
    			    unwatchProperty(name, oldWatchedValue);
    			
    			if (newWatchedValue != null)
    				watchProperty(name, newWatchedValue);
            }
			
			var propertyAdded : Boolean = !_descriptor.hasOwnProperty(name);
			
			_descriptor[name]		= name;
			_nameToProperty[name]	= newValue;
			
			if (propertyAdded)
				_propertyAdded.execute(this, name, dataDescriptor[name], newValue);
			else
				_propertyChanged.execute(this, name, dataDescriptor[name], newValue);
			
			return this;
		}
		
		public function setProperties(properties : Object) : DataProvider
		{
			var propertyName : String = null;
			
			if (properties is DataProvider)
			{
				var provider : DataProvider = properties as DataProvider;
				
				for each (propertyName in provider.dataDescriptor)
					setProperty(propertyName, provider[propertyName]);
			}
			else 
				for (propertyName in properties)
					setProperty(propertyName, properties[propertyName]);
				
			return this;
		}
		
		protected function watchProperty(name : String, property : IWatchable) : void
		{
			if (property != null)
			{
				var newPropertyNames : Vector.<String> = _propertyToNames[property] as Vector.<String>;
				
				if (newPropertyNames == null)
				{
					_propertyToNames[property] = newPropertyNames = new <String>[name];
					property.changed.add(propertyChangedHandler);
				}
				else
					newPropertyNames.push(name);
			}
		}
        
        protected function unwatchProperty(name : String, property : IWatchable) : void
        {
            var oldPropertyNames	: Vector.<String>	= _propertyToNames[property];
            
            if (oldPropertyNames.length == 1)
            {
                property.changed.remove(propertyChangedHandler);
                delete _propertyToNames[property];
            }
            else
            {
                var numPropertyNames : uint = oldPropertyNames.length - 1;
                
                oldPropertyNames[oldPropertyNames.indexOf(name)] = oldPropertyNames[numPropertyNames];
                oldPropertyNames.length = numPropertyNames;
            }
        }
		
		public function removeProperty(name : String) : DataProvider
		{
			if (_descriptor.hasOwnProperty(name))
			{
				var oldMonitoredValue	: IWatchable		= _nameToProperty[name] as IWatchable;
				var oldPropertyNames	: Vector.<String>	= _propertyToNames[oldMonitoredValue];
				var bindingName			: String			= _descriptor[name];
				
				delete _descriptor[name];
				delete _nameToProperty[name];
				
				if (oldMonitoredValue != null)
					unwatchProperty(name, oldMonitoredValue);
				
				_propertyRemoved.execute(this, name, bindingName, oldMonitoredValue);
			}
			
			return this;
		}
		
		public function removeAllProperties() : DataProvider
		{
			for (var propertyName : String in _nameToProperty)
				removeProperty(propertyName);
			
			return this;
		}
        
        public function dispose() : void
        {
            removeAllProperties();
            _propertyToNames = null;
            _nameToProperty = null;
            _descriptor = null;
            _propertyAdded = null;
            _propertyChanged = null;
            _propertyRemoved = null;
        }
		
		public function propertyExists(name : String) : Boolean
		{
			if (_descriptor)
				return _descriptor.hasOwnProperty(name);
			else
				return false;
		}
		
		public function invalidate() : DataProvider
		{
			_propertyChanged.execute(this, null);
			
			return this;
		}
		
		public function clone() : IDataProvider
		{
			switch (_usage)
			{
				case DataProviderUsage.EXCLUSIVE:
					return new DataProvider(_nameToProperty, _name + '_cloned', _usage);
				case DataProviderUsage.SHARED:
					return this;
				case DataProviderUsage.MANAGED:
					throw new Error('This dataprovider is managed, and must not be cloned');
				default:
					throw new Error('Unkown usage value');
			}
		}
		
		private function propertyChangedHandler(source : IWatchable) : void
		{
			var names		: Vector.<String>	= _propertyToNames[source];
			var numNames	: uint				= names.length;
			
			for (var nameId : uint = 0; nameId < numNames; ++nameId)
			{
				var propertyName : String = names[nameId];
				
				_propertyChanged.execute(this, propertyName, dataDescriptor[propertyName], source);
			}
		}
	}
}
