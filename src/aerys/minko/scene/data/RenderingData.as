package aerys.minko.scene.data
{
	import aerys.minko.render.effect.IEffect;

	public final class RenderingData
	{
		private var _style	: StyleStack	= new StyleStack();
		private var _effect	: IEffect		= null;
		
		public function get styleStack()	: StyleStack	{ return _style; }
		public function get effect()		: IEffect		{ return _effect; }
		
		public function set effect(value : IEffect) : void
		{
			_effect = value;
		}
	}
}