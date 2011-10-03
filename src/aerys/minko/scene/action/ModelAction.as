package aerys.minko.scene.action
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.Model;
	import aerys.minko.scene.visitor.ISceneVisitor;

	public final class ModelAction implements IAction
	{
		private static const TYPE		: uint			= ActionType.RECURSE;

		private static var _instance	: ModelAction	= null;

		public static function get modelAction() : ModelAction
		{
			return _instance || (_instance = new ModelAction());
		}

		public function get type() : uint		{ return TYPE; }

		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var model : Model = scene as Model;

			if (!model.visible)
				return false;

			model.textures && visitor.visit(model.textures);
			model.mesh && visitor.visit(model.mesh);

			return true;
		}
	}
}