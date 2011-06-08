package aerys.minko.scene.action.texture
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.AnimatedTexture;
	import aerys.minko.scene.node.texture.ColorTexture;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	import flash.utils.getTimer;
	
	public class AnimatedTextureAction implements IAction
	{
		private static const NO_TEXTURE	: ColorTexture	= new ColorTexture(0);
		
		private var _time			: int		= 0;
		private var _lastFrameId	: int		= 0;
		
		public function get type() : uint
		{
			return ActionType.RECURSE;
		}
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			//if (renderer && renderer.frameId != _lastFrameId)
			{
				var t  		:  int 				= getTimer();
				var texture : AnimatedTexture 	= scene as AnimatedTexture;
								
				if (t - _time > (1000. / texture.framerate))
				{
//					_lastFrameId = renderer.frameId;
					texture.nextFrame();
					_time = t;
				}
				
				var currentFrame : int = texture.currentFrame;
				
				if (currentFrame < texture.numChildren)
					visitor.visit(texture.getChildAt(currentFrame));
				else
					visitor.visit(NO_TEXTURE);
				
			}
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
	}
}