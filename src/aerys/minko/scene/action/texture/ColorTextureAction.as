package aerys.minko.scene.action.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.IRenderer;
	import aerys.minko.scene.action.ActionType;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.node.IScene;
	import aerys.minko.scene.node.texture.ColorTexture;
	import aerys.minko.scene.visitor.ISceneVisitor;
	
	public class ColorTextureAction implements IAction
	{
		private static const TYPE	: uint		= ActionType.UPDATE_STYLE;
		
		private static var _instance	: ColorTextureAction	= null;
		
		public static function get colorTextureAction() : ColorTextureAction
		{
			return _instance || (_instance = new ColorTextureAction());
		}
		
		public function get type() : uint		{ return TYPE; }
		
		public function prefix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
		
		public function infix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			var texture : ColorTexture = scene as ColorTexture;
			var color	: uint = texture.color;
			if (!texture)
				throw new Error();
			
			var alpha	: uint = (color >> 24) & 0xff
			var red		: uint = (color >> 16) & 0xff
			var green	: uint = (color >> 8) & 0xff
			var blue	: uint = color & 0xff;
			
			color = (red << 24) | (green << 16) | (blue << 8) | (alpha); 
				
			visitor.renderingData.styleStack.set(BasicStyle.DIFFUSE_COLOR, color);
			
			return true;
		}
		
		public function postfix(scene : IScene, visitor : ISceneVisitor, renderer : IRenderer) : Boolean
		{
			return true;
		}
	}
}