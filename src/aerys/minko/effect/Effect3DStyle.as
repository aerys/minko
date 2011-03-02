package aerys.minko.effect
{
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
		
	public final class Effect3DStyle implements IEffect3DStyle
	{
		private var _target		: IEffect3DStyle	= null;
		private var _properties	: Object			= new Object();
		private var _append		: Object			= new Object();
		
		public function Effect3DStyle()
		{
			super();
		}
		
		public function override(style : IEffect3DStyle = null) : IEffect3DStyle
		{
			var target : IEffect3DStyle = _target;
			
			_target = style;
			
			if (target == null)
				for (var name:String in _append)
					(_properties[name] as Array).length = _append[name];
			
			return target || this;
		}
		
		public function clear() : void 
		{
			_properties	= new Object();
			_append		= new Object();
			_target		= null;
		}
		
		public function get(name : String, defaultValue : * = undefined) : *
		{
			var value : * = _properties[name] !== undefined
							? _properties[name]
							: (_target ? _target.get(name, defaultValue) : defaultValue);
			
			if (value === undefined)
				throw new Error("Unable to read a style that was never set if no default value is provided.");
			
			return value;
		}
		
		public function set(name : String, value : *) : IEffect3DStyle
		{
			_properties[name] = value;
			
			return this;
		}
		
		public function append(name : String, value : *) : IEffect3DStyle 
		{
			var a : Array = get(name, null);
			
			if (!a)
			{
				a = new Array();
				set(name, a);
			}
			
			if (!_append.hasOwnProperty(name))
				_append[name] = a.length;
			
			a.push(value);
			
			return this;
		}
		
	}
}