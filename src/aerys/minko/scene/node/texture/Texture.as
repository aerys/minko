package aerys.minko.scene.node.texture
{
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.scene.action.texture.TextureAction;
	import aerys.minko.scene.node.AbstractScene;
	
	public class Texture extends AbstractScene implements ITexture
	{
		protected var _version		: uint				= 0;
		
		private var _resource		: TextureResource	= null;
		private var _styleProperty	: uint				= 0;
		
		public function get styleProperty() : int
		{
			return _styleProperty;
		}
		
		public function set styleProperty(value : int) : void
		{
			_styleProperty = value;
		}
		
		public function get resource() : TextureResource
		{
			return _resource;
		}
		
		public function get version() : uint
		{
			return _version;
		}
		
		public function Texture(resource 		: TextureResource	= null,
								styleProperty	: int				= -1)
		{
			_resource = resource || new TextureResource();
			_styleProperty = styleProperty != -1 ? styleProperty : BasicStyle.DIFFUSE;
			
			actions.push(TextureAction.textureAction);
		}
	}
}