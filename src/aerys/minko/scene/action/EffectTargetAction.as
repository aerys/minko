package aerys.minko.scene.action
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectTarget;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class EffectTargetAction implements IAction
	{
		private static const TYPE		: uint					= ActionType.RENDER;
		
		private static var _instance	: EffectTargetAction	= null;
		
		public static function get effectTargetAction() : EffectTargetAction
		{
			return _instance || (_instance = new EffectTargetAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var effects 	: Vector.<IEffect>	= (scene as IEffectTarget).effects;
			var numEffects	: int				= effects.length;
			
			for (var i : int = 0; i < numEffects; ++i)
				visitor.renderingData.effects.push(effects[i]);
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var numEffects	: int	= (scene as IEffectTarget).effects.length;
			
			visitor.renderingData.effects.length -= numEffects;
			
			return true;
		}
	}
}