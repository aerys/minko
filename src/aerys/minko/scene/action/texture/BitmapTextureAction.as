package aerys.minko.scene.action.texture
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.BitmapTexture;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class BitmapTextureAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.UPDATE_STYLE;
		
		private static var _instance	: BitmapTextureAction	= null;
		
		public static function get bitmapTextureAction() : BitmapTextureAction
		{
			return _instance || (_instance = new BitmapTextureAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var texture : BitmapTexture = scene as BitmapTexture;
			
			if (!texture)
				throw new Error();
			
			visitor.renderingData.styleStack.set(texture.styleProperty, texture.ressource);
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
	}
}