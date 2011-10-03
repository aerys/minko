package aerys.minko.scene.action.texture
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.DynamicTexture;
	import aerys.minko.scene.visitor.ISceneVisitor;

	import flash.display.BitmapData;
	import flash.display.DisplayObject;

	/**
	 * The DynamicTextureAction takes care of the rasterizing process
	 * of a DynamicTexture source DisplayObject object.
	 *
	 * @author Jean-Marc Le Roux
	 *
	 */
	public final class DynamicTextureAction implements IAction
	{
		private var _lastFrame	: int			= uint(-1);
		private var _bitmapData	: BitmapData	= null;

		public function get type() : uint
		{
			return ActionType.UPDATE_STYLE;
		}

		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			if (renderer && renderer.frameId != _lastFrame)
			{
				var dynamicTexture 	: DynamicTexture 	= scene as DynamicTexture;
				var source			: DisplayObject		= dynamicTexture.source;

				_lastFrame = renderer.frameId;

				_bitmapData ||= new BitmapData(source.width, source.height);
				_bitmapData.draw(source);

				dynamicTexture.updateFromBitmapData(_bitmapData, false);
			}

			return true;
		}
	}
}