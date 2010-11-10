package aerys.minko
{
	import aerys.common.log.ILogger;
	import aerys.common.log.LogLevel;
	
	public class Minko
	{
		//{ region consts
		public static const VERSION	: String		= "0.1b";
		public static const URL		: String		= "http://minko.in";
		//} endregion
		
		//{ region vars
		private static var _logger		: ILogger	= null;
		private static var _debugLevel	: uint		= LogLevel.DISABLED;
		//} endregion
		
		//{ region getters/setters
		public static function set logger(value : ILogger)	: void
		{
			_logger = value;
			logMessage("Minko v" + VERSION + " logger set and initialized");
		}
		public static function set debugLevel(value : uint)	: void	{ _debugLevel = value; }
		
		public static function get debugLevel() : uint				{ return _debugLevel; }
		//} endregion
		
		//{ region methods
		public static function log(myMessage : Object, myTarget : Object = null) : void
		{
			if (_logger != null && _debugLevel & LogLevel.LOG)
				_logger.log(myMessage, myTarget);
		}
		
		public static function logMessage(myMessage : Object, myTarget : Object = null) : void
		{
			if (_logger != null && _debugLevel & LogLevel.MESSAGE)
				_logger.logMessage(myMessage, myTarget);
		}
		
		public static function logWarning(myMessage : Object, myTarget : Object = null) : void
		{
			if (_logger != null && _debugLevel & LogLevel.WARNING)
				_logger.logWarning(myMessage, myTarget);
		}
		
		public static function logError(myMessage : Object, myTarget : Object = null) : void
		{
			if (_logger != null && _debugLevel & LogLevel.ERROR)
				_logger.logError(myMessage, myTarget);
			
			throw new Error(myMessage);
		}
		//} endregion
	}
}