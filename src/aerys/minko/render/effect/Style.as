package aerys.minko.render.effect
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.type.IVersionable;

	public final class Style implements IVersionable
	{
		use namespace minko_render;

		private static var _nextId		: uint		= 0;
		private static var _nameToId	: Object	= new Object();
		private static var _idToName	: Array		= new Array();

		minko_render var	_data 		: Array		= new Array();

		private var		_version 	: uint		= 0;

		public static function getStyleName(styleId : uint) : String
		{
			return _idToName[styleId];
		}

		public static function getStyleId(styleName : String) : uint
		{
			if (_nameToId.hasOwnProperty(styleName))
				return _nameToId[styleName];

			_idToName[_nextId] = styleName;
			_nameToId[styleName] = _nextId;

			return _nextId++;
		}

		public function get version() 	: uint		{ return _version; }

		public function isSet(styleId : int) : Boolean
		{
			return _data.hasOwnProperty(styleId);
		}

		public function get(styleId : int, defaultValue : Object = null) : Object
		{
			if (!_data.hasOwnProperty(styleId))
				throw new Error("Style '" + getStyleName(styleId) + "' is not set.");

			return _data[styleId];
		}

		public function set(styleId : int, value : Object) : Style
		{
			_data[styleId] = value;
			++_version;

			return this;
		}

		public function unset(styleId : int) : Style
		{
			delete _data[styleId];
			++_version;

			return this;
		}

		public function clear() : Style
		{
			_data.length = 0;;
			_version = 0;

			return this;
		}
	}
}
