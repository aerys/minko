package aerys.minko.query.renderdata.style
{
	import aerys.common.IVersionnable;

	public class Style3D implements IVersionnable
	{
		internal var	_data : Object;
		private var		_version : uint;
		
		public function get version() : uint	
		{
			return _version;
		}
		
		public function Style3D()
		{
			_data		= new Object();
			_version	= 0;
		}
		
		public function get(name : String, defaultValue : * = undefined) : * 
		{
			var data : * = _data[name];
			if (data != undefined)
				return data;
			
			throw new Error();
		}
		
		public function set(name : String, value : *) : Style3D 
		{
			_data[name] = value;
			++_version;
			return this;
		}
		
		public function clear() : Style3D
		{
			_data = new Object();
			_version = 0;
			return this;
		}
		
	}
}
