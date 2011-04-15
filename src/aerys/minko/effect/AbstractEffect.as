package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;

	public class AbstractEffect implements IEffect
	{
		private var _passes	: Vector.<IEffectPass>	= new Vector.<IEffectPass>();
		private var _style	: Style					= new Style();
		
		public function get style() 	: Style					{ return _style; }
		public function get passes() 	: Vector.<IEffectPass>	{ return _passes; }
		
		public function AbstractEffect()
		{
		}
		
		public function begin(renderer : IRenderer, style : StyleStack) : void
		{
			// NOTHING
		}
		
		public function end(renderer : IRenderer, style : StyleStack) : void
		{
			// NOTHING
		}
	}
}