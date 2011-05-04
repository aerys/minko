package aerys.minko.effect
{
	import aerys.common.IVersionnable;

	public class Style implements IVersionnable
	{
		internal var	_data : Object;
		private var		_version : uint;
		
		public function get version() : uint	
		{
			return _version;
		}
		
		public function Style()
		{
			_data		= new Object();
			_version	= 0;
		}
		
		public function get(name : String, defaultValue : Object = undefined) : Object 
		{
			if (_data[name] != undefined)
				return _data[name];
			
			throw new Error();
		}
		
		public function set(name : String, value : Object) : Style 
		{
			_data[name] = value;
			++_version;
			return this;
		}
		
		public function clear() : Style
		{
			_data = new Object();
			_version = 0;
			return this;
		}
		
	}
}
