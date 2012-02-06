package aerys.minko.type.data
{
	import aerys.minko.type.Signal;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;
	
	public class List extends Proxy implements IDataProvider
	{
		private var _changed	: Signal	= new Signal();
		private var _data		: Array		= [];
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function List(...elements)
		{
			_data.push.apply(null, elements);
		}
		
		override flash_proxy function getProperty(name:*):*
		{
			return _data[parseInt(name)];
		}
		
		override flash_proxy function setProperty(name:*, value:*):void
		{
			var index : int = 0;
			
			_data[index] = value;
			_changed.execute(this, parseInt(name));
		}
	}
}