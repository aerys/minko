package aerys.minko.scene.action.transform
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public final class PopTransformAction implements IAction
	{
		private static const TYPE		: uint					= ActionType.UPDATE_LOCAL_DATA;
		
		private static var _instance	: PopTransformAction	= null;
		
		public static function get popTransformAction() : PopTransformAction
		{
			return _instance || (_instance = new PopTransformAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			visitor.transformData.world.pop();
			
			return true;
		}
	}
}