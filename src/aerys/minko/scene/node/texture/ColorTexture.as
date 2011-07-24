package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.ressource.IRessource;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.texture.ColorTextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
	public class ColorTexture extends AbstractScene implements ITexture
	{
		private var _version : uint;
		private var _color : uint;
		
		public function get color() : uint { return _color; }
		public function get styleProperty() : int { return BasicStyle.DIFFUSE_COLOR; }
		public function get ressource() : TextureRessource { return null; }
		
		public function set color(v : uint) : void
		{
			_color = v;
			++_version
		}
		
		public function ColorTexture(color : uint = 0xffffff)
		{
			_color		= color;
			
			actions[0]	= ColorTextureAction.colorTextureAction;
		}
		
		public function get version() : uint
		{
			return _version;
		}
	}
}