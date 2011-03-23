package aerys.minko.effect
{
	import aerys.common.Factory;

	public class Effect3DStyle
	{
		internal var _data : Object;
		
		public function Effect3DStyle()
		{
			_data	= new Object();
		}
		
		public function get(name : String, defaultValue : * = undefined) : * 
		{
			var data : * = _data[name];
			if (data != undefined)
				return data;
			
			throw new Error();
		}
		
		public function set(name : String, value : *) : Effect3DStyle 
		{
			_data[name] = value;
			return this;
		}
		
		public function clear() : Effect3DStyle
		{
			_data = new Object();
			return this;
		}
		
	}
}
