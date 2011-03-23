package aerys.minko.scene.group
{
	import aerys.minko.effect.IEffect3D;
	import aerys.minko.query.rendering.RenderingQuery;

	/**
	 * EffectGroup3D enables 
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EffectGroup3D extends StyleGroup3D
	{
		private var _effects	: Vector.<IEffect3D>	= new Vector.<IEffect3D>();
		
		public function get effects() : Vector.<IEffect3D>
		{
			return _effects;
		}
		
		public function EffectGroup3D(...children)
		{
			super(children);
		}
		
		override protected function acceptRenderingQuery(q : RenderingQuery) : void
		{
			var numEffects		: int					= _effects.length;
			var queryEffects	: Vector.<IEffect3D>	= q.effects;
			var numQueryEffects	: int					= queryEffects.length;
			
			// push effects
			for (var i : int = 0; i < numEffects; ++i)
				queryEffects[int(numQueryEffects + i)] = _effects[i];
			
			super.acceptRenderingQuery(q);
			
			// pop effects
			queryEffects.length = numQueryEffects;
		}
	}
}