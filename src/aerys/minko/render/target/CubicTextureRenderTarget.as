package aerys.minko.render.target
{
	import aerys.minko.render.resource.texture.CubicTextureResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.display3D.Context3D;

	public class CubicTextureRenderTarget extends AbstractRenderTarget
	{
		private var _cubicTextureResource	: CubicTextureResource;
		private var _surfaceSelector		: uint;
		
		override public function get textureResource() : ITextureResource
		{ 
			return _cubicTextureResource; 
		}
		
		public function get cubicTextureResource() : CubicTextureResource
		{ 
			return _cubicTextureResource; 
		}
		
		public function get surfaceSelector() : uint
		{
			return _surfaceSelector;
		}
		
		public function CubicTextureRenderTarget(resource			: CubicTextureResource,
												 surfaceSelector	: uint,
												 backgroundColor	: uint		= 0, 
												 useDepthAndStencil	: Boolean	= true, 
												 antiAliasing		: int		= 0)
		{
			var size : uint = resource.size;
			
			super(size, size, backgroundColor, useDepthAndStencil, antiAliasing);
			
			_cubicTextureResource	= resource;
			_surfaceSelector		= surfaceSelector;
			
			if ((size & (size - 1)) != 0)
				throw new ArgumentError('Texture width must be a power of 2.');
			
			if (resource == null)
				throw new ArgumentError('Resource cannot be null.');
			
			if (surfaceSelector > 5)
				throw new RangeError('Surfaceselector is between 0 and 5.');
			
			if (size > 2048)
				throw new RangeError('Maximum texture size is 2048.');
		}
		
		override public function configureContext(context : Context3D):void
		{
			context.setRenderToTexture(
				_cubicTextureResource.getNativeTexture(context),
				_useDepthAndStencil,
				_antiAliasing,
				_surfaceSelector);
		}
	}
}