package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.IResource;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.IAction;
	import aerys.minko.scene.action.texture.ColorTextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
	public class ColorTexture extends AbstractScene implements ITexture
	{
		private var _version 		: uint	= 0;
		private var _color 			: uint	= 0;
		private var _styleProperty	: int	= 0;
		
		/**
		 * The texture color in ARGB format
		 * 
		 * @return current texture color
		 */		
		public function get color() 		: uint 				{ return _color; }
		public function get styleProperty() : int 				{ return _styleProperty; }
		public function get ressource() 	: TextureResource 	{ return null; }
		
		/**
		 * Set texture color in ARGB format
		 *  
		 * @param value the new color
		 */		
		public function set color(value : uint) : void
		{
			_color = value;
			++_version
		}
		
		public function set styleProperty(value : int) : void
		{
			_styleProperty = value;
		}
		
		public function ColorTexture(argb : uint = 0xffffffff)
		{
			_color			= argb;
			_styleProperty	= BasicStyle.DIFFUSE;
			
			actions[0]		= ColorTextureAction.colorTextureAction;
		}
		
		public function get version() : uint
		{
			return _version;
		}
	}
}