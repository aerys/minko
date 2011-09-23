package aerys.minko.scene.node.group
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IRenderingEffect;
	import aerys.minko.scene.action.effect.PopEffectAction;
	import aerys.minko.scene.action.effect.PushEffectAction;
	import aerys.minko.scene.node.IEffectScene;

	/**
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class EffectGroup extends Group implements IEffectScene
	{
		private var _effect	: IRenderingEffect	= null;
		
		public function get effect() : IRenderingEffect	{ return _effect; }
		
		public function set effect(value : IRenderingEffect) : void
		{
			_effect = value;
		}
		
		public function EffectGroup(effect : IRenderingEffect = null, ...children)
		{
			super(children);
			
			_effect = effect;
			
			actions.unshift(new PushEffectAction());
			actions.push(new PopEffectAction());
		}
	}
}