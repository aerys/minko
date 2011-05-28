package aerys.minko.render.renderer
{
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.renderer.state.RenderState;
	import aerys.minko.type.Factory;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.utils.getTimer;

	public class DirectRenderer implements IRenderer
	{
		use namespace minko;
		use namespace minko_render;
		
		private static const RENDER_STATE	: Factory			= Factory.getFactory(RenderState);
		private static const DEBUG			: Boolean			= false;
		
		private var _context		: Context3D					= null;
		private var _currentState	: RenderState				= new RenderState();
		private var _actualState	: RenderState				= null;
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport					= null;
		private var _drawingTime	: int						= 0;
		private var _frame			: uint						= 0;
		
		public function get state() 		: RenderState	{ return _currentState; }
		public function get numTriangles()	: uint			{ return _numTriangles; }
		public function get viewport()		: Viewport		{ return _viewport; }
		public function get drawingTime()	: int			{ return _drawingTime; }
		public function get frameId()		: uint			{ return _frame; }
		
		public function DirectRenderer(viewport : Viewport, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_context.enableErrorChecking = DEBUG;
		}

		public function drawTriangles(offset		: uint	= 0,
									  numTriangles	: int	= -1) : void
		{
			var indexStream : IndexStream 	= _currentState._indexStream; 
			var t 			: int 			= getTimer();
		
			_currentState.prepareContext(_context, _actualState);
			_context.drawTriangles(indexStream.getIndexBuffer3D(_context),
								   offset,
								   numTriangles);
			
			_drawingTime += getTimer() - t;
			_numTriangles += numTriangles == -1 ? indexStream.length / 3. : numTriangles;
		}
	
		public function clear(red 		: Number	= 0.,
							  green 	: Number	= 0.,
							  blue 		: Number	= 0.,
							  alpha 	: Number	= 1.,
							  depth 	: Number	= 1.,
							  stencil	: uint		= 0,
							  mask		: uint		= 0xffffffff) : void
		{
			_context.clear(red, green, blue, alpha, depth, stencil, mask);
			
			_numTriangles = 0;
			_drawingTime = 0;
			
			_actualState = null;
			_currentState = null;
		}
		
		public function present() : void
		{
			var time : int = getTimer();
			
			_context.present();
			
			_drawingTime += getTimer() - time;
			++_frame;
		}
		
		public function drawToBitmapData(bitmapData : BitmapData) : void
		{
			_context.drawToBitmapData(bitmapData);
		}
		
		public function begin() : void
		{
			_currentState = RENDER_STATE.create(true) as RenderState;
			_currentState.clear();
		}
		
		public function end() : void
		{
			_actualState = _currentState;
		}
		
	}
}