package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;

	/**
	 * EffectGroup3D enables 
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EffectGroup extends StyleGroup implements IEffectTarget
	{
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		
		public function get effects() : Vector.<IEffect>
		{
			return _effects;
		}
		
		public function EffectGroup(...children)
		{
			super(children);
		}
		
//		override protected function visitedByRenderingVisitor(q : RenderingVisitor) : void
//		{
//			var numEffects		: int					= _effects.length;
//			var queryEffects	: Vector.<IEffect>	= q.effects;
//			var numQueryEffects	: int					= queryEffects.length;
//			
//			// push effects
//			for (var i : int = 0; i < numEffects; ++i)
//				queryEffects[int(numQueryEffects + i)] = _effects[i];
//			
//			super.visitedByRenderingVisitor(q);
//			
//			// pop effects
//			queryEffects.length = numQueryEffects;
//		}
	}
}