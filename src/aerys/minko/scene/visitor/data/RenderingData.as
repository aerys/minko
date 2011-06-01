package aerys.minko.scene.visitor.data
{
	import aerys.minko.render.effect.IEffect;

	public final class RenderingData
	{
		private var _style		: StyleStack		= new StyleStack();
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		
		public function get styleStack()	: StyleStack		{ return _style; }
		public function get effects()		: Vector.<IEffect>	{ return _effects; }
		
		public function RenderingData()
		{
		}
		
		public function clear(defaultEffect : IEffect) : void
		{
			if (defaultEffect)
			{
				_effects.length = 1;
				_effects[0] = defaultEffect;
			}
			else
			{
				_effects.length = 0;
			}
		}
	}
}