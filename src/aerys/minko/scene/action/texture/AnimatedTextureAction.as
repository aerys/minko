package aerys.minko.scene.action.texture
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.AnimatedTexture;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.scene.visitor.ISceneVisitor;

	import flash.display.BitmapData;
	import flash.utils.getTimer;

	public final class AnimatedTextureAction implements IAction
	{
		private static const NO_TEXTURE	: ITexture	= new BitmapTexture(new BitmapData(1, 1, false, 0));

		private var _time			: int		= 0;
		private var _lastFrameId	: int		= 0;

		public function get type() : uint		{ return ActionType.RECURSE; }

		public function run(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			if (renderer)
			{
				var texture : AnimatedTexture 	= scene as AnimatedTexture;

				if (renderer.frameId != _lastFrameId)
				{
					var t  	:  int 	= getTimer();

					if (t - _time > (1000. / texture.framerate))
					{
						_lastFrameId = renderer.frameId;
						texture.nextFrame();
						_time = t;
					}
				}

				var currentFrame : int = texture.currentFrame;

				if (currentFrame < texture.numChildren)
					visitor.visit(texture.getChildAt(currentFrame));
				else
					visitor.visit(NO_TEXTURE);
			}

			return true;
		}
	}
}