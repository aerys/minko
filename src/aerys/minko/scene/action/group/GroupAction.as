package aerys.minko.scene.action.group
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.IGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class GroupAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.RECURSE;
		
		private static var _instance	 :  GroupAction	= null;
		
		public static function get groupAction()  :  GroupAction
		{
			return _instance || (_instance = new GroupAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var group 		 :  IGroup 	= scene as IGroup;
			var numChildren  :  int 	= group.numChildren;
			
			for (var childIndex : uint = 0; childIndex < numChildren; ++childIndex)
				visitor.visit(group.getChildAt(childIndex));
			
			return true;
		}
	}
}