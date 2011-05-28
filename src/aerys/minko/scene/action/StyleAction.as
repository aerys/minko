package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IStyled;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class StyleAction implements IAction
	{
		private static var _instance	: StyleAction	= null;
		
		public static function get styleAction() : StyleAction
		{
			return _instance || (_instance = new StyleAction());
		}
		
		public function StyleAction()
		{
		}
		
		public function get name() : String
		{
			return "StyleAction";
		}
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var styled : IStyled = scene as IStyled;
			
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
			
			visitor.renderingData.styleStack.pop();
			
			return true;
		}
	}
}