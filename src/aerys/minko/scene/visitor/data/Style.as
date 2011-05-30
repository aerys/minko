package aerys.minko.scene.visitor.data
{
	import aerys.minko.type.IVersionnable;

	public class Style implements IVersionnable
	{
		private static var _nextId	: uint	= 0;
		
		internal var	_data 		: Array	= new Array();
		
		private var		_version 	: uint	= 0;
		
		public static function getStyleId(styleName : String) : uint
		{
			return _nextId++;
		}
		
		public function get version() : uint	
		{
			return _version;
		}
		
		public function Style()
		{
		}
		
		public function get(styleId : int, defaultValue : Object = null) : Object 
		{
			var data : Object = _data[styleId];
			
			if (data !== null)
				return data;
			
			throw new Error();
		}
		
		public function set(styleId : int, value : Object) : Style 
		{
			_data[styleId] = value;
			++_version;
			
			return this;
		}
		
		public function clear() : Style
		{
			_data = new Array();
			_version = 0;
			
			return this;
		}
		
	}
}
