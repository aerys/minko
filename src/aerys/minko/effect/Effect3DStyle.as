package aerys.minko.effect
{
	import aerys.minko.effect.basic.BasicEffect3D;
	import aerys.minko.effect.basic.BasicStyle3D;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
		
	public final class Effect3DStyle implements IEffect3DStyle
	{
		private var _target		: IEffect3DStyle	= null;
		
		private var _properties	: Object			= new Object();
		private var _tmp		: Object			= new Object();
		
		private var _append		: Object			= new Object();
		
		public function Effect3DStyle()
		{
			super();
		}
		
		public function override(style : IEffect3DStyle = null) : IEffect3DStyle
		{
			if (style === null)
			{
				var target : IEffect3DStyle = _target;
				
				for (var name : String in _append)
					((_properties[name] || _tmp[name]) as Array).length = _append[name];
				
				_target = null;
				
				return target;
			}
			else
			{
				_target = style;
				
				return this;
			}
		}
		
		public function clear() : void 
		{
			_properties	= new Object();
			//_append		= new Object();
			_tmp		= new Object();
			_target		= null;
		}
		
		public function get(name : String, defaultValue : * = undefined) : *
		{
			if (_target && _tmp[name] !== undefined)
				return _tmp[name];
			
			if (_properties[name] !== undefined)
				return _properties[name];
			
			if (_target)
				return _target.get(name, defaultValue);
			
			if (defaultValue === undefined)
				throw new Error("Unable to read a style that was never set if no default value is provided.");

			return defaultValue;
		}
		
		public function set(name : String, value : *) : IEffect3DStyle
		{
			// if we are in the style stack, we set the temporary table
			if (_target)
				_tmp[name] = value;
			else
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
			
			//if (!_append.hasOwnProperty(name))
				_append[name] = a.length;
			
			a.push(value);
			
			return this;
		}
		
	}
}