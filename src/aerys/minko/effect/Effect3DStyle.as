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
			_target		= null
		}
		
		public function has(name : String) : Boolean 
		{
			return _properties.hasOwnProperty(name);
		}
		
		public function get(name : String, readonly : Boolean = true) : *
		{
			return _properties[name] || (_target ? _target.get(name) : null);
		}
		
		public function set(name : String, value : *) : IEffect3DStyle
		{
			_properties[name] = value;
			
			return this;
		}
		
		public function append(name : String, value : *) : IEffect3DStyle 
		{
			var a:Array = get(name);
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