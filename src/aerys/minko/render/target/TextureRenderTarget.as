package aerys.minko.render.target
{
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.display3D.Context3D;

	public class TextureRenderTarget extends AbstractRenderTarget
	{
		private var _textureResource : TextureResource	= null;

		override public function get textureResource() : ITextureResource
		{ 
			return _textureResource; 
		}
		
		public function TextureRenderTarget(width				: uint, 
											height				: uint, 
											backgroundColor		: uint					= 0, 
											useDepthAndStencil	: Boolean				= true, 
											antiAliasing		: int					= 0,
											resource			: TextureResource	= null)
		{
			super(width, height, backgroundColor, useDepthAndStencil, antiAliasing);
			
			if ((width & (width - 1)) != 0)
				throw new Error('Texture width must be a power of 2.');
			
			if ((height & (height - 1)) != 0)
				throw new Error('Texture height must be a power of 2.');
			
			_textureResource = resource || new TextureResource();
			_textureResource.setSize(width, height);
		}
		
		override public function configureContext(context : Context3D) : void
		{
			context.setRenderToTexture(
				_textureResource.getNativeTexture(context),
				_useDepthAndStencil,
				_antiAliasing);
		}
	}
}