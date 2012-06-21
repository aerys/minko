package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	import aerys.minko.type.enum.DataProviderUsage;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public dynamic class DataProvider extends Proxy implements IDataProvider
	{
		private var _usage			: uint		= 1;
		private var _name			: String	= null;
		private var _descriptor		: Object	= {};
		private var _data			: Object	= {};
		
		private var _changed		: Signal	= new Signal('DataProvider.changed');
		
		public function get usage() : uint
		{
			return _usage;
		}
		
		public function get dataDescriptor() : Object
		{
			return _descriptor;
		}
		
		public function get changed() : Signal
		{
			return _changed;
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
			if (properties)
				for (var propertyName : String in properties)
					setProperty(propertyName, properties[propertyName]);
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
			return _data[name];
		}
		
		public function setProperty(name : String, value : Object) : DataProvider
		{
			var propertyExists : Boolean	= _descriptor.hasOwnProperty(name);
			
			_descriptor[name] = name;
			_data[name] = value;
			
			if (!propertyExists)
				_changed.execute(this, 'dataDescriptor');
			else
				_changed.execute(this, name);
			
			return this;
		}
		
		public function setProperties(properties : Object) : DataProvider
		{
			for (var propertyName : String in properties)
				setProperty(propertyName, properties[propertyName]);
				
			return this;
		}
		
		public function removeProperty(name : String) : DataProvider
		{
			delete _descriptor[name];
			delete _data[name];
			
			_changed.execute(this, 'dataDescriptor');
			
			return this;
		}
		
		public function removeAllProperties() : DataProvider
		{
			for (var propertyName : String in _data)
				removeProperty(propertyName);
			
			return this;
		}
		
		public function propertyExists(name : String) : Boolean
		{
			return _descriptor.hasOwnProperty(name);
		}
		
		public function invalidate() : DataProvider
		{
			_changed.execute(this, null);
			
			return this;
		}
		
		public function clone() : IDataProvider
		{
			switch (_usage)
			{
				case DataProviderUsage.EXCLUSIVE:
				case DataProviderUsage.SHARED:
					return new DataProvider(_data, _name + '_cloned', _usage);
				
				case DataProviderUsage.MANAGED:
					throw new Error('This dataprovider is managed, and must not be cloned');
					
				default:
					throw new Error('Unkown usage value');
			}
		}
	}
}