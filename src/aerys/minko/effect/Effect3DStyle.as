package aerys.minko.effect
{
	import flash.utils.flash_proxy;
	
	public final class Effect3DStyle implements IEffect3DStyle
	{
		private var _target		: IEffect3DStyle	= null;
		private var _properties	: Object			= new Object();
		
		public function Effect3DStyle()
		{
			super();
		}
		
		public function override(style : IEffect3DStyle = null) : IEffect3DStyle
		{
			var target : IEffect3DStyle = _target;
			
			_target = style;
			
			return target || this;
		}
		
		public function get(name : String) : *
		{
			return _properties[name] || (_target ? _target.get(name) : null);
		}
		
		public function set(name : String, value : *) : IEffect3DStyle
		{
			_properties[name] = value;
			
			return this;
		}
	}
}