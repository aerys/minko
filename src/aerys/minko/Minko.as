package aerys.minko
{
	import aerys.minko.type.log.ILogger;

	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.net.URLRequest;
	import flash.net.navigateToURL;

	public class Minko
	{
		public static const VERSION	: String		= "1.0 beta";
		public static const URL		: String		= "http://aerys.in/minko";

		private static var _logger		: ILogger	= null;
		private static var _debugLevel	: uint		= 0; // LogLevel.DISABLED;
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
		}
		public static function set debugLevel(value : uint)	: void	{ _debugLevel = value; }

		public static function get debugLevel() : uint				{ return _debugLevel; }

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