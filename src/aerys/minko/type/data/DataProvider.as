package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public dynamic class DataProvider extends Proxy implements IDynamicDataProvider
	{
		private var _descriptor			: Object	= {};
		private var _data				: Object	= {};
		
		private var _locked				: Boolean	= false;
		private var _invalid			: Boolean	= false;
		
		private var _propertyAdded		: Signal	= new Signal();
		private var _propertyRemoved	: Signal	= new Signal();
		private var _changed			: Signal	= new Signal();
		
		public function get dataDescriptor() : Object
		{
			return _descriptor;
		}
		
		public function get propertyAdded() : Signal
		{
			return _propertyAdded;
		}
		
		public function get propertyRemoved() : Signal
		{
			return _propertyRemoved;
		}
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get locked() : Boolean
		{
			return _locked;
		}
		
		public function lock() : void
		{
			_locked = true;
		}
		
		public function unlock() : void
		{
			_locked = false;
			
			if (_invalid)
			{
				_invalid = false;
				_changed.execute(this, null);
			}
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
				_propertyAdded.execute(this, name);
			
			if (_locked)
				_invalid = true;
			else
				_changed.execute(this, name);
			
			return this;
		}
		
		public function removeProperty(name : String) : DataProvider
		{
			delete _descriptor[name];
			delete _data[name];
			
			_propertyRemoved.execute(this, name);
			
			return this;
		}
		
		public function invalidate() : void
		{
			_changed.execute(this, null);
		}
	}
}