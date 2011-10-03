package aerys.minko.scene.action.style
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.IStylableScene;
	import aerys.minko.scene.visitor.ISceneVisitor;

	public final class PopStyleAction implements IAction
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
			var styled : IStylableScene = scene as IStylableScene;

			if (styled.styleEnabled)
				visitor.renderingData.styleStack.pop();

			return true;
		}
	}
}