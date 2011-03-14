package aerys.minko.effect
{
	public final class Effect3DStyle
	{
		private static const APPEND_DATA	: Object	= new Object();	
		
		private var _target		: Effect3DStyle		= null;
		private var _properties	: Object			= new Object();
		private var _append		: Object			= new Object();
		
		public function Effect3DStyle()
		{
			super();
		}
		
		public function push(style : Effect3DStyle) : void
		{
			if (_target)
				style._target = _target;
			_target = style;
		}
		
		public function pop() : void
		{
			var append : Object = _target._append;
			
			for (var name : String in append)
				(_properties[name] as Array).length = append[name];
					
			_target = _target._target;
		}
		
		public function clear() : void 
		{
			_properties	= new Object();
			_target		= null;
		}
		
		public function get(styleName : String, defaultValue : * = undefined) : *
		{
			var style : Effect3DStyle 	= this;
			var value : *				= null;
				
			while (style != null && (value = style._properties[styleName]) === undefined)
				style = style._target;
				
			if (style != null)
				return value;
			
			if (defaultValue === undefined)
				throw new Error("Unable to read a style that was never set if no default value is provided.");

			return defaultValue;
		}
		
		public function set(name : String, value : *) : Effect3DStyle
		{
			_properties[name] = value;
			
			return this;
		}
		
		public function append(styleName : String, value : *) : Effect3DStyle 
		{
			var data : Array = null;
			
			if ((data = APPEND_DATA[styleName]) === undefined)
				APPEND_DATA[styleName] = data = new Array();
			
			_properties[styleName] = data;
			(_target || this)._append[styleName] = data.length;
			data.push(value);
			
			return this;
		}
		
	}
}