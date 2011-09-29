package aerys.minko.render.renderer
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.Viewport;
	import aerys.minko.type.Factory;
	import aerys.minko.type.log.DebugLevel;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.getTimer;
	
	public class DefaultRenderer implements IRenderer
	{
		use namespace minko;
		use namespace minko_render;
	
		private static const SORT	: Boolean					= true;
		
		private var _context		: Context3D					= null;
		private var _currentState	: RendererState				= null;
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport					= null;
		private var _drawingTime	: int						= 0;
		private var _frame			: uint						= 0;
	
		private var _states			: Vector.<RendererState>	= new Vector.<RendererState>();
		private var _numStates		: int						= 0;
		
		public function get numTriangles()	: uint			{ return _numTriangles; }
		public function get viewport()		: Viewport		{ return _viewport; }
		public function get drawingTime()	: int			{ return _drawingTime; }
		public function get frameId()		: uint			{ return _frame; }
		
		public function DefaultRenderer(viewport : Viewport, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_context.enableErrorChecking = (Minko.debugLevel & DebugLevel.RENDERER) != 0;
		}
		
		public function pushState(state : RendererState) : void
		{
			_states[int(_numStates++)] = state;
			_currentState = state;
		}
		
		public function drawTriangles(offset : uint = 0, numTriangles : int = -1) : void
		{
			_currentState.drawTriangles(offset, numTriangles);
		}
		
		public function reset()  :void
		{
			_numTriangles = 0;
			_drawingTime = 0;
			
			_currentState = null;
			_numStates = 0;
		}
		
		public function drawToBackBuffer() : void
		{
			var time : int = getTimer();
			
			if (SORT && _numStates > 1)
				RendererState.sort(_states, _numStates);
			
			_currentState = null;
			for (var i : int = 0; i < _numStates; ++i)
			{
				var state	: RendererState = _states[i];

				_numTriangles += state.apply(_context, _currentState);
				_currentState = state;
			}
			
			_drawingTime += getTimer() - time;
		}
		
		public function present() : void
		{
			var time : int = getTimer();
			
			if (_numStates != 0)
				_context.present();
			
			_drawingTime += getTimer() - time;
			++_frame;
		}
		
		public function dumpBackbuffer(bitmapData : BitmapData) : void
		{
			var time : int = getTimer();
			
			if (_numStates != 0)
				_context.drawToBitmapData(bitmapData);
			
			_drawingTime += getTimer() - time;
			++_frame;
		}
	}
}