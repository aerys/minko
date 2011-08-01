package aerys.minko.scene.action.mesh
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class PopMeshSkinAction implements IAction
	{
		private static var _instance : PopMeshSkinAction	= null;
		
		public static function get popMeshSkinAction() : PopMeshSkinAction
		{
			return _instance || (_instance = new PopMeshSkinAction());
		}
		
		public function get type() : uint		{ return ActionType.UPDATE_STYLE; }
		
		public function run(scene:IScene, visitor:ISceneVisitor, renderer:IRenderer):Boolean
		{
			visitor.renderingData.styleStack.pop();
			
			return true;
		}
	}
}