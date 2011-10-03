package aerys.minko.scene.action.texture
{
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.ITexture;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public final class TextureAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.UPDATE_STYLE;
		
		private static var _instance	: TextureAction	= null;
		
		public static function get textureAction() : TextureAction
		{
			return _instance || (_instance = new TextureAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function run(scene		: IScene, 
							visitor		: ISceneVisitor, 
							renderer	: IRenderer) : Boolean
		{
			var texture : ITexture = scene as ITexture;
			
			if (!texture)
				throw new Error();
			
			visitor.renderingData.styleData.set(texture.styleProperty, texture.resource);
			
			return true;
		}
	}
}