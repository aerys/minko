package aerys.minko.scene.action
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class EffectAction implements IAction
	{
		private var _effects	: Vector.<IEffect>	= null;
		
		public function EffectAction()
		{
		}
		
		public function get name() : String
		{
			return "EffectAction";
		}
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			_effects = (scene as IEffectTarget).effects;
			
			var numEffects	 : int	= _effects.length;
			
			for (var i : int = 0; i < numEffects; ++i)
				visitor.renderingData.effects.push(_effects[i]);
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var numEffects	 : int	= _effects.length;
			
			visitor.renderingData.effects.length -= numEffects;
			
			return true;
		}
	}
}