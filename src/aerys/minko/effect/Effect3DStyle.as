package aerys.minko.effect
{
	import aerys.common.Factory;

	public class Effect3DStyle
	{
		internal var _dataContainer : Object;
		internal var _listContainer : Object;
		
		public function Effect3DStyle()
		{
			_dataContainer = new Object();
			_listContainer = new Object();
		}
		
		public function get(name : String, defaultValue : * = undefined) : * 
		{
			if (_dataContainer[name] != undefined)
				return _dataContainer[name];
			
			if (_listContainer[name] != undefined)
				return _listContainer[name];
			
			if (defaultValue !== undefined)
				return defaultValue;
			
			throw new Error();
		}
		
		public function set(name : String, value : *) : Effect3DStyle 
		{
			_dataContainer[name] = value;
			return this;
		}
		
		public function append(name : String, value : * = undefined) : Effect3DStyle
		{
			throw new Error('implement me');
			return this;
		}
		
		public function clear() : Effect3DStyle
		{
			_dataContainer = new Object();
			_listContainer = new Object();
			return this;
		}
		
	}
}
