package aerys.minko.render.renderer
{
	import aerys.minko.ns.minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.type.Factory;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.getTimer;
	
	public class DefaultRenderer implements IRenderer
	{
		use namespace minko;
		use namespace minko_render;
	
		private static const RENDER_STATE	: Factory			= Factory.getFactory(RendererState);
		private static const SORT			: Boolean			= true;
		private static const DEBUG			: Boolean			= true;
		
		private var _context		: Context3D					= null;
		private var _currentState	: RendererState				= null;
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport					= null;
		private var _drawingTime	: int						= 0;
		private var _frame			: uint						= 0;
	
		private var _states			: Vector.<RendererState>	= new Vector.<RendererState>();
		private var _numStates		: int						= 0;
		
		public function get state() 		: RendererState	{ return _currentState; }
		public function get numTriangles()	: uint			{ return _numTriangles; }
		public function get viewport()		: Viewport		{ return _viewport; }
		public function get drawingTime()	: int			{ return _drawingTime; }
		public function get frameId()		: uint			{ return _frame; }
		
		public function DefaultRenderer(viewport : Viewport, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_context.enableErrorChecking = DEBUG;
		}
		
		public function begin()	: void
		{
			_currentState = RENDER_STATE.create(true) as RendererState;
			_currentState.clear();
		}
		
		public function end() : void
		{
			_states[int(_numStates++)] = _currentState;
			_currentState = null;
		}
		
		public function drawTriangles(offset : uint = 0, numTriangles : int = -1) : void
		{
			_currentState.offsets.push(offset);
			_currentState.numTriangles.push(numTriangles);
		}
		
		public function clear(red		: Number	= 0.,
							  green		: Number	= 0.,
							  blue		: Number	= 0.,
							  alpha		: Number	= 1.,
							  depth		: Number	= 1.,
							  stencil	: uint		= 0,
							  mask		: uint		= 0xffffffff)  :void
		{
			_context.clear(red, green, blue, alpha, depth, stencil, mask);
			
			_numTriangles = 0;
			_drawingTime = 0;
			
			_currentState = null;
			_numStates = 0;
		}
		
		public function drawToBackBuffer() : void
		{
			var time : int = getTimer();
			
			if (SORT && _numStates > 1)
				RendererState.sort(_states);
			
			var actualState : RendererState = null;
			
			for (var i : int = 0; i < _numStates; ++i)
			{
				var state			: RendererState = _states[i];
				var offsets 		: Vector.<uint>	= state.offsets;
				var numTriangles 	: Vector.<int> 	= state.numTriangles;
				var numCalls 		: int 			= offsets.length;
				
				state.prepareContext(_context, actualState);
				
				for (var j : int = 0; j < numCalls; ++j)
				{
					var iStream : IndexStream	= state._indexStream;
					var iBuffer : IndexBuffer3D = iStream.getIndexBuffer3D(_context);
					var count	: int			= numTriangles[j];
					
					_numTriangles += count == -1
									 ? state._indexStream.length / 3.
									 : count;
					
					_context.drawTriangles(iBuffer, offsets[j], count);
				}
				
				actualState = state;
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
			
			_context.drawToBitmapData(bitmapData);
			
			_drawingTime += getTimer() - time;
			++_frame;
		}
	}
}