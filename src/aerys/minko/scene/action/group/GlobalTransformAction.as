package aerys.minko.scene.action.group
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.group.GlobalTransformGroup;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.type.math.Matrix3D;
	
	public class GlobalTransformAction implements IAction
	{
		private static var _instance	: GlobalTransformAction	= null;
		
		public static function get globalTransformAction() : GlobalTransformAction
		{
			return _instance || (_instance = new GlobalTransformAction());
		}
		
		public function get type()	: uint		{ return ActionType.RECURSE; }
		
		public function run(scene		: IScene,
							visitor		: ISceneVisitor,
							renderer	: IRenderer) : Boolean
		{
			var globalTransformGroup : GlobalTransformGroup = GlobalTransformGroup(scene);
			
			Matrix3D.copy(visitor.transformData.world, globalTransformGroup.transform);
			
			return true;
		}
	}
}