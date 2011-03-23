package aerys.minko
{
	import aerys.common.log.ILogger;
	import aerys.common.log.LogLevel;
	
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.net.URLRequest;
	import flash.net.navigateToURL;
	
	public class Minko
	{
		public static const VERSION	: String		= "0.01a";
		public static const URL		: String		= "http://aerys.in/minko";
		
		private static var _logger		: ILogger	= null;
		private static var _debugLevel	: uint		= LogLevel.DISABLED;
		private static var _logo		: Sprite	= null;
		
		public static function get logo() : Sprite
		{
			if (!_logo)
			{
				_logo = new Sprite();
				_logo.graphics.beginFill(0x7f7f7f, .85);
				_logo.graphics.lineTo(50, 0);
				_logo.graphics.lineTo(25, 25);
				_logo.graphics.endFill();
				_logo.visible = true;
				_logo.useHandCursor = true;
				_logo.buttonMode = true;
				_logo.addEventListener(MouseEvent.CLICK, logoClickHandler);	
			}
			
			return _logo;
		}
		
		private static function logoClickHandler(event : Event) : void
		{
			navigateToURL(new URLRequest(URL), "_blank");
		}
		
		public static function set logger(value : ILogger)	: void
		{
			_logger = value;
			logMessage("Minko v" + VERSION + " logger set and initialized");
		}
		public static function set debugLevel(value : uint)	: void	{ _debugLevel = value; }
		
		public static function get debugLevel() : uint				{ return _debugLevel; }
		
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
	}
}