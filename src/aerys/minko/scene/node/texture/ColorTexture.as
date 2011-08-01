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
		
		/**
		 * The texture color in ARGB format
		 * 
		 * @return current texture color
		 */		
		public function get color() : uint { return _color; }
		
		public function get styleProperty() : int { return BasicStyle.DIFFUSE; }
		
		public function get ressource() : TextureRessource { return null; }
		
		/**
		 * Set texture color in ARGB format
		 *  
		 * @param v the new color
		 */		
		public function set color(v : uint) : void
		{
			_color = v;
			++_version
		}
		
		public function ColorTexture(argb : uint = 0xffffff)
		{
			_color		= argb;
			
			actions[0]	= ColorTextureAction.colorTextureAction;
		}
		
		public function get version() : uint
		{
			return _version;
		}
	}
}