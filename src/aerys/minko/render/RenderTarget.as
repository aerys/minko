package aerys.minko.render
{
	
	import aerys.minko.ns.minko;
	
	import flash.display3D.textures.TextureBase;
	
	public class RenderTarget
	{
		use namespace minko;
		
		private var _size				: uint			= 0;
		
		minko   var _texture			: TextureBase	= null;
		private var _useDepthAndStencil	: Boolean		= false;
		private var _antiAliasing		: int			= 0.;
		private var _surface			: int			= 0;
		
		public function RenderTarget(size 				: uint,
									 useDepthAndStencil	: Boolean,
									 antiAliasing		: int	= 0)
		{
			_size = size;
			_useDepthAndStencil = useDepthAndStencil;
			_antiAliasing = antiAliasing;
		}
	
		public function get size()					: uint			{ return _size; }
		public function get texture() 				: TextureBase	{ return _texture; }
		public function get useDepthAndStencil()	: Boolean		{ return _useDepthAndStencil; }
		public function get antiAliasing()			: int			{ return _antiAliasing; }
	}
}