package aerys.minko.render
{
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.resource.texture.TextureResource;

	/**
	 * RenderTarget objects can be used to render in the backbuffer or
	 * in textures.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 * @see aerys.minko.render.resource.texture.ITextureResource
	 * 
	 */
	public final class RenderTarget
	{
		private var _width				: uint				= 0;
		private var _height				: uint				= 0;
		private var _resource			: ITextureResource	= null;
		private var _surfaceSelector	: uint				= 0;
		private var _useDepthAndStencil	: Boolean			= false;
		private var _antiAliasing		: int				= 0;
		private var _backgroundColor	: uint				= 0;
		
		public function get width() : uint
		{
			return _width;
		}
		public function set width(value : uint) : void
		{
			_width = value;
			
			if (_resource)
				_resource.setSize(value, _height);
		}
		
		public function get height() : uint
		{
			return _height;
		}
		public function set height(value : uint) : void
		{
			_height = value;
			
			if (_resource)
				_resource.setSize(_width, value);
		}
		
		public function get textureResource() : ITextureResource
		{
			return _resource;
		}
		
		public function get surfaceSelector() : uint
		{
			return _surfaceSelector;
		}
		
		public function get useDepthAndStencil() : Boolean
		{
			return _useDepthAndStencil;
		}
		
		public function get antiAliasing() : int
		{
			return _antiAliasing;
		}
		
		public function get backgroundColor() : uint
		{
			return _backgroundColor;
		}
		public function set backgroundColor(value : uint) : void
		{
			_backgroundColor = value;
		}
		
		public function RenderTarget(width				: uint,
									 height				: uint,
									 resource			: ITextureResource	= null,
									 surfaceSelector	: uint				= 0,
									 backgroundColor	: uint				= 0,
									 useDepthAndStencil	: Boolean			= true,
									 antiAliasing		: int				= 0)
		{
			_width = width;
			_height = height;
			_resource = resource;
			_surfaceSelector = surfaceSelector;
			_backgroundColor = backgroundColor;
			_useDepthAndStencil = useDepthAndStencil;
			_antiAliasing = antiAliasing;
			
			if (_resource && (_resource.width != _width || _resource.height != _height))
				throw new Error(
					'The texture resource and the render target must have the same size.'
				);
		}
	}
}