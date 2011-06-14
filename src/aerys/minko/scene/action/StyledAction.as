package aerys.minko.scene.action
{
	import aerys.minko.render.effect.Style;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IStyled;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class StyledAction implements IAction
	{
		private static const TYPE		: uint			= ActionType.UPDATE_STYLE;
		
		private static var _instance	: StyledAction	= null;
		
		public static function get styledAction() : StyledAction
		{
			return _instance || (_instance = new StyledAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var styled : IStyled = scene as IStyled;
			
			if (styled.styleEnabled)
				visitor.renderingData.styleStack.push(styled.style);
			
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var styled : IStyled = scene as IStyled;
			
			if (styled.styleEnabled)
				visitor.renderingData.styleStack.pop();
			
			return true;
		}
	}
}