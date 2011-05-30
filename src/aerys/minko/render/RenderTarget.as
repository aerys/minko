package aerys.minko.render
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.ressource.TextureRessource;
	
	public class RenderTarget
	{
		use namespace minko;
		
		public static const BACKBUFFER	: uint = 0;
		public static const TEXTURE		: uint = 1;
		
		private var _width				: uint				= 0;
		private var _height				: uint				= 0;
		
		minko   var _textureRessource	: TextureRessource	= null;
		private var _useDepthAndStencil	: Boolean			= false;
		private var _antiAliasing		: int				= 0.;
		private var _surface			: int				= 0;
		private var _backgroundColor	: uint				= 0;
		
		public function RenderTarget(type				: uint,
									 width				: uint,
									 height				: uint,
									 backgroundColor	: uint		= 0,
									 useDepthAndStencil	: Boolean	= true,
									 antiAliasing		: int		= 0)
		{
			_backgroundColor	= backgroundColor;
			_useDepthAndStencil	= useDepthAndStencil;
			_antiAliasing		= antiAliasing;
			
			if (type == BACKBUFFER)
			{
				_width				= width;
				_height				= height;
				_textureRessource	= null;
			}
			else if (type == TEXTURE)
			{
				if (width != height) 
					throw new Error('Width and height must be equal for texture rendering.');
				
				if ((width & (width - 1)) != 0)
					throw new Error('Texture size must be a power of 2.');
				
				_textureRessource = new TextureRessource();
				_textureRessource.setSize(width, height);
			}
		}
		
		public function get type()					: uint				{ return _textureRessource == null ? BACKBUFFER : TEXTURE; }
		public function get width()					: uint				{ return _width; }
		public function get height()				: uint				{ return _height; }
		
		public function get textureRessource() 		: TextureRessource	{ return _textureRessource; }
		public function get useDepthAndStencil()	: Boolean			{ return _useDepthAndStencil; }
		public function get antiAliasing()			: int				{ return _antiAliasing; }
		public function get backgroundColor()		: uint				{ return _backgroundColor; }
	}
}