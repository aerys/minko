package aerys.minko.scene.data
{
	import aerys.minko.render.effect.IEffect;

	public final class RenderingData
	{
		private var _style	: StyleStack		= new StyleStack();
		private var _effect	: Vector.<IEffect>	= new Vector.<IEffect>;
		
		public function get styleStack()	: StyleStack		{ return _style; }
		public function get effect()		: Vector.<IEffect>	{ return _effect; }
	}
}