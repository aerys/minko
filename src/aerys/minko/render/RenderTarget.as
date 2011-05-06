package aerys.minko.render
{
	
	import aerys.minko.ns.minko;
	import aerys.minko.render.ressource.TextureRessource;
	
	public class RenderTarget
	{
		use namespace minko;
		
		private var _size				: uint				= 0;
		
		minko   var _textureRessource	: TextureRessource	= null;
		private var _useDepthAndStencil	: Boolean			= false;
		private var _antiAliasing		: int				= 0.;
		private var _surface			: int				= 0;
		
		public function RenderTarget(size 				: uint,
									 useDepthAndStencil	: Boolean,
									 antiAliasing		: int	= 0)
		{
			_size				= size;
			_useDepthAndStencil	= useDepthAndStencil;
			_antiAliasing		= antiAliasing;
			_textureRessource	= new TextureRessource();
			_textureRessource.setSize(size, size);
		}
		
		public function get size()					: uint				{ return _size; }
		public function get textureRessource() 		: TextureRessource	{ return _textureRessource; }
		public function get useDepthAndStencil()	: Boolean			{ return _useDepthAndStencil; }
		public function get antiAliasing()			: int				{ return _antiAliasing; }
	}
}