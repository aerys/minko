package aerys.minko.scene.action
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class EffectTargetAction implements IAction
	{
		private static const TYPE		: uint					= ActionType.UPDATE_EFFECTS;

		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		
		public function get type() : uint		{ return TYPE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			_effects.push(visitor.renderingData.effect);
			
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var effect : IEffect = (scene as IEffectTarget).effect;
			
			if (effect)
				visitor.renderingData.effect = effect;
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			visitor.renderingData.effect = _effects.pop();
			
			return true;
		}
	}
}