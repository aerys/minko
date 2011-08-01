package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;

	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EffectGroup extends StyleGroup implements IEffectTarget
	{
		private var _effect	: IEffect	= null;
		
		public function get effect() : IEffect	{ return _effect; }
		
		public function set effect(value : IEffect) : void
		{
			_effect = value;
		}
		
		public function EffectGroup(...children)
		{
			super(children);
			
			actions.unshift(new PushEffectAction());
			actions.push(new PopEffectAction());
		}
	}
}