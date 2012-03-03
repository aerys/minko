package aerys.minko
{
	import aerys.minko.type.log.ILogger;

	public final class Minko
	{
		private static const VERSION	: String	= "2.0b";
		
		private static var _logger		: ILogger	= null;
		private static var _debugLevel	: uint		= 0; // LogLevel.DISABLED;

		public static function set logger(value : ILogger)	: void
		{
			_logger = value;
		}
		
		public static function get debugLevel() : uint
		{
			return _debugLevel;
		}
		public static function set debugLevel(value : uint)	: void
		{
			_debugLevel = value;
		}
		
		public static function log(type : uint, message : Object, target : Object = null) : void
		{
			if (_debugLevel & type)
			{
				if (_logger != null)
					_logger.log(message);
				else
					trace(message);
			}
		}		
	}
}