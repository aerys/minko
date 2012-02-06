package aerys.minko.render
{
	import flash.display.BitmapData;
	import flash.display3D.Context3D;

	public final class Renderer
	{
		private var _frameNumber	: uint			= 0;
		private var _context		: Context3D		= null;
		
		private var _renderingList	: RenderingList	= new RenderingList();		
		private var _numTriangles	: uint			= 0;
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function Renderer(context : Context3D)
		{
			_context = context;
		}
		
		public function pushDrawCall(state 	: RendererState,
									 call	: DrawCall) : void
		{
			_renderingList.pushDrawCall(state, call);
		}
		
		public function render() : void
		{
			_renderingList.render(_context);
		}
		
		public function present() : void
		{
			_context.present();
		}
		
		public function drawToBitmapData(bitmapData : BitmapData) : void
		{
			_context.drawToBitmapData(bitmapData);
		}
		
		public function clear() : void
		{
			_context.clear();
			_renderingList.clear();
			
			++_frameNumber;
			_numTriangles = 0;
		}
		
		public function configureBackBuffer(width				: uint,
											height 				: uint,
											antiAliasing		: uint		= 0,
											useDepthAndStencil	: Boolean	= true) : void
		{
			_context.configureBackBuffer(
				width,
				height,
				antiAliasing,
				useDepthAndStencil
			);
		}
	}
}