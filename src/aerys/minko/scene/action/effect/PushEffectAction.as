package aerys.minko.scene.action.effect
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class PushEffectAction implements IAction
	{
		public function get type() : uint		{ return ActionType.UPDATE_SELF; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var effect : IEffect = (scene as IEffectTarget).effect;
			
			if (effect)
				visitor.renderingData.effects.push(effect);
			
			return true;
		}
	}
}