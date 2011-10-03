package aerys.minko.scene.action.style
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IStylableScene;
	import aerys.minko.scene.visitor.ISceneVisitor;

	public final class PushStyleAction implements IAction
	{
		private static const TYPE		: uint				= ActionType.UPDATE_STYLE;

		private static var _instance	: PushStyleAction	= null;

		public static function get pushStyleAction() : PushStyleAction
		{
			return _instance || (_instance = new PushStyleAction());
		}

		public function get type() : uint		{ return TYPE; }

		public function run(scene:IScene, visitor:ISceneVisitor, renderer:IRenderer):Boolean
		{
			var styled : IStylableScene = scene as IStylableScene;

			if (styled.styleEnabled)
				visitor.renderingData.styleStack.push(styled.style);

			return true;
		}
	}
}