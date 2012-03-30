package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public dynamic class DataProvider extends Proxy implements IDataProvider
	{
		private var _descriptor	: Object	= {};
		private var _data		: Object	= {};
		
		private var _changed	: Signal	= new Signal('DataProvider.changed');
		
		public function get dataDescriptor() : Object
		{
			return _descriptor;
		}
		
		public function get changed() : Signal
		{
			return _changed;
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
		
		public function getProperty(name : String) : Object
		{
			return _data[name];
		}
		
		public function setProperty(name : String, value : Object) : DataProvider
		{
			var propertyExists : Boolean	= _descriptor.hasOwnProperty(name);
			
			_descriptor[name] = name;
			_data[name] = value;
			
			if (!propertyExists)
				_changed.execute(this, "dataDescriptor");
			
			_changed.execute(this, name);
			
			return this;
		}
		
		public function removeProperty(name : String) : DataProvider
		{
			delete _descriptor[name];
			delete _data[name];
			
			_changed.execute(this, "dataDescriptor");
			
			return this;
		}
		
		public function invalidate() : void
		{
			_changed.execute(this, null);
		}
	}
}