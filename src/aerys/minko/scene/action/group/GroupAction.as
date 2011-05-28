package aerys.minko.scene.action.group
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class GroupAction implements IAction
	{
		private static var _instance	: GroupAction	= null;
		
		public static function get groupAction() : GroupAction
		{
			return _instance || (_instance = new GroupAction());
		}
		
		public function GroupAction()
		{
		}
		
		public function get name():String
		{
			return null;
		}
		
		public function prefix(scene:IScene, visitor:ISceneVisitor, renderer:IRenderer):Boolean
		{
			return true;
		}
		
		public function infix(scene:IScene, visitor:ISceneVisitor, renderer:IRenderer):Boolean
		{
			var group 		: IGroup 	= scene as IGroup;
			var numChildren : int 		= group.numChildren;
			
			for (var childIndex : uint = 0; childIndex < numChildren; ++childIndex)
				visitor.visit(group.getChildAt(childIndex));
			
			return true;
		}
		
		public function postfix(scene:IScene, visitor:ISceneVisitor, renderer:IRenderer):Boolean
		{
			return true;
		}
	}
}