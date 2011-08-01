package aerys.minko.scene.action.effect
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	
	public class PushEffectAction implements IAction
	{
		private var _effects	: Vector.<IEffect>	= new Vector.<IEffect>();
		
		public function get type() : uint		{ return ActionType.UPDATE_SELF; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			_effects.push(visitor.renderingData.effect);
			
			return true;
		}
	}
}