package aerys.minko.effect
{
	import aerys.common.IVersionnable;

	public final class Style3D implements IVersionnable
	{
		private static const APPEND_DATA	: Object	= new Object();	
		
		private var _version	: uint		= 0;
		
		private var _target		: Style3D	= null;
		private var _properties	: Object	= new Object();
		private var _append		: Object	= new Object();
		
		public function get version() : uint	{ return _version; }
		
		public function Style3D()
		{
			super();
		}
		
		public function push(style : Style3D) : void
		{
			if (_target)
				style._target = _target;
			_target = style;
		}
		
		public function pop() : void
		{
			var append : Object = _target._append;
			
			/*for (var name : String in append)
				(_properties[name] as Array).length = append[name];*/
					
			_target = _target._target;
		}
		
		public function clear() : void 
		{
			_properties	= new Object();
			_target		= null;
		}
		
		public function get(styleName : String, defaultValue : * = undefined) : *
		{
			var style : Style3D 	= this;
			var value : *				= null;
				
			while (style != null && (value = style._properties[styleName]) === undefined)
				style = style._target;
				
			if (style != null)
				return value;
			
			if (defaultValue === undefined)
				throw new Error("Unable to read style '" + styleName
								+ "' and no default value was provided.");

			return defaultValue;
		}
		
		public function set(name : String, value : *) : Style3D
		{
			_properties[name] = value;
			++_version;
			
			return this;
		}
		
		public function append(styleName : String, value : *) : Style3D 
		{
			var data : Array = null;
			
			++_version;
			
			if ((data = APPEND_DATA[styleName]) === undefined)
				APPEND_DATA[styleName] = data = new Array();
			
			_properties[styleName] = data;
			(_target || this)._append[styleName] = data.length;
			data.push(value);
			
			return this;
		}
		
	}
}