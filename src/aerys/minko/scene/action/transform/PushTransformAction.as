package aerys.minko.scene.action.transform
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.ITransformableScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public final class PushTransformAction implements IAction
	{
		private static const TYPE		: uint					= ActionType.UPDATE_TRANSFORM_DATA;
		
		private static var _instance	: PushTransformAction	= null;
		
		public static function get pushTransformAction() : PushTransformAction
		{
			return _instance || (_instance = new PushTransformAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var transformable : ITransformableScene = scene as ITransformableScene;
			
			if (!transformable)
				throw new Error("TransformAction can only be applied to ITransformable nodes.");
			
			visitor.transformData.world.push()
							 	   .prepend(transformable.transform);
			
			return true;
		}
	}
}