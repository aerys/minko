package aerys.minko.scene.action.group
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.MaterialGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class MaterialGroupAction implements IAction
	{
		private static var _instance	: MaterialGroupAction	= null;
		
		public static function get materialGroupAction() : MaterialGroupAction
		{
			return _instance || (_instance = new MaterialGroupAction());
		}
		
		public function get type() : uint		{ return ActionType.RECURSE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var materialGroup : MaterialGroup	= MaterialGroup(scene);
			
			visitor.visit(materialGroup.textures);
			
			return true;
		}
	}
}