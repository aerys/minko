package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.scene.action.EffectTargetAction;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;

	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EffectGroup extends StyleGroup implements IEffectTarget
	{
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		
		public function get effects() : Vector.<IEffect>	{ return _effects; }
		
		public function EffectGroup(...children)
		{
			super(children);
			
			actions.unshift(EffectTargetAction.effectTargetAction);
		}
	}
}