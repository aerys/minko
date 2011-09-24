package aerys.minko.scene.data
{
	import aerys.minko.render.effect.IEffect;

	public final class RenderingData
	{
		private var _style		: StyleData		= new StyleData();
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>;
		
		public function get styleStack()	: StyleData		{ return _style; }
		public function get effects()		: Vector.<IEffect>	{ return _effects; }
	}
}