package aerys.minko.render.target
{
	import aerys.minko.render.IRenderer3D;
	
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.TextureBase;
	
	public class TextureTarget implements IRenderTarget3D
	{
		private var _size		: uint			= 0;
		private var _frame		: uint			= 0;
		private var _texture	: TextureBase	= null;
		
		public function TextureTarget(size : uint)
		{
			_size = size;
		}
	
		public function get size() : uint
		{
			return _size;
		}
		
		public function get texture() : TextureBase
		{
			return _texture;
		}
		
		public function prepare(renderer : IRenderer3D) : void
		{
			_texture ||= renderer.createTexture(_size,
											    _size,
												Context3DTextureFormat.BGRA,
												true);
			
			if (_frame != renderer.frameId)
			{
				_frame = renderer.frameId;
		
				renderer.beginRenderToTexture(_texture, true);
				renderer.clear(0., 0., 0., 0.);
				renderer.endRenderToTexture();
			}
		}
	}
}