package aerys.minko.render.target
{
	import aerys.minko.render.resource.texture.ITextureResource;
	
	import flash.display3D.Context3D;

	public class AbstractRenderTarget
	{
		protected var _width				: uint		= 0;
		protected var _height				: uint		= 0;
		protected var _useDepthAndStencil	: Boolean	= false;
		protected var _antiAliasing			: int		= 0;
		protected var _backgroundColor		: uint		= 0;

		public function get width()					: uint				{ return _width; }
		public function get height()				: uint				{ return _height; }
		public function get useDepthAndStencil()	: Boolean			{ return _useDepthAndStencil; }
		public function get antiAliasing()			: int				{ return _antiAliasing; }
		public function get backgroundColor()		: uint				{ return _backgroundColor; }
		public function get textureResource()		: ITextureResource	{ return null; }
		
		public function AbstractRenderTarget(width				: uint,
											 height				: uint,
											 backgroundColor	: uint		= 0,
											 useDepthAndStencil	: Boolean	= true,
											 antiAliasing		: int		= 0)
		{
			_backgroundColor	= backgroundColor;
			_useDepthAndStencil	= useDepthAndStencil;
			_antiAliasing		= antiAliasing;
			_width				= width;
			_height				= height;
		}
		
		public function configureContext(context : Context3D) : void
		{
			throw new Error('Must be overriden');
		}
	}
}
