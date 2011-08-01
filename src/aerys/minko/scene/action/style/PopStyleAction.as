package aerys.minko.scene.action.style
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IStyledScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	
	public class PopStyleAction implements IAction
	{
		private static const TYPE		: uint				= ActionType.UPDATE_STYLE;
		
		private static var _instance	: PopStyleAction	= null;
		
		public static function get popStyleAction() : PopStyleAction
		{
			return _instance || (_instance = new PopStyleAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene		: IScene,
							visitor		: ISceneVisitor,
							renderer	: IRenderer) : Boolean
		{
			var styled : IStyledScene = scene as IStyledScene;
			
			if (styled.styleEnabled)
				visitor.renderingData.styleStack.pop();
			
			return true;
		}
	}
}