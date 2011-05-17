package aerys.minko.render
{
	import aerys.common.Factory;
	import aerys.minko.ns.minko;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.visitor.data.TransformManager;
	import aerys.minko.stage.Viewport;
	import aerys.minko.type.stream.IndexStream;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.IndexBuffer3D;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;

	public class DirectRenderer implements IRenderer
	{
		use namespace minko;
		
		private static const RENDER_SESSION	: Factory			= Factory.getFactory(RenderSession);
		private static const RENDER_STATE	: Factory			= Factory.getFactory(RenderState);
		private static const DIRECT			: Boolean			= false;
		private static const SORT			: Boolean			= true;
		private static const DEBUG			: Boolean			= false;
		
		private var _context		: Context3D					= null;
		private var _state			: RenderState				= new RenderState();
		private var _currentState	: RenderState				= new RenderState();
		private var _transform		: TransformManager			= new TransformManager();
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport					= null;
		private var _drawingTime	: int						= 0;
		private var _frame			: uint						= 0;
		
		private var _rtSessions		: Dictionary				= null;
		private var _sessions		: Vector.<RenderSession>	= new Vector.<RenderSession>();
		private var _currentSession	: RenderSession				= new RenderSession();
		private var _numSessions	: int						= 0;
		
		public function get state() 		: RenderState	{ return _state; }
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

		public function drawTriangles(firstIndex	: uint	= 0,
									  count			: int	= -1) : void
		{
			var indexStream : IndexStream = _state.indexStream; 
		
//			count ||= indexStream.length / 3;
			
			/*if (indexStream.length == 0 || count == 0)
				return ;*/
			
			if (!DIRECT)
			{
				_currentSession.renderState = _state;
				_currentSession.offsets.push(firstIndex);
				_currentSession.numTriangles.push(count);
			}
			else
			{
				var t : int = getTimer();
			
				_state.prepareContext(_context);
				_context.drawTriangles(indexStream.getIndexBuffer3D(_context),
									   firstIndex,
									   count);
				_drawingTime += getTimer() - t;
				
			}
			
			_numTriangles += count == -1 ? indexStream.length / 3. : count;
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
			
//			_rtSessions = new Dictionary(true);
			
			end();
		}
		
		public function present() : void
		{
			var time : int = getTimer();
			
			if (SORT)
			{
				// FIXME crappy coding.
				_sessions = _sessions.sort(function(rs1 : RenderSession, rs2 : RenderSession) : Number {
					return 1000 * (rs2.renderState.priority - rs1.renderState.priority);
				});
			}
			
			for (var i : int = 0; i < _numSessions; ++i)
			{
				_currentSession = _sessions[i];

				var state	: RenderState 	= _currentSession.renderState;
				
				state.prepareContext(_context, _currentState);
				
				var offsets 		: Vector.<uint>	= _currentSession.offsets;
				var numTriangles 	: Vector.<int> 	= _currentSession.numTriangles;
				var numCalls 		: int 			= offsets.length;
				
				for (var j : int = 0; j < numCalls; ++j)
				{
					var ib : IndexBuffer3D = state.indexStream.getIndexBuffer3D(_context);
					
					if (ib)
						_context.drawTriangles(ib, offsets[j], numTriangles[j]);
				}

				RENDER_SESSION.free(_currentSession);
				RENDER_STATE.free(_currentState);
				
				_currentState = state;
			}
			
			_currentSession = null;
			_numSessions = 0;
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
			
		}
		
		public function end() : void
		{
			if (DIRECT)
			{
				_state.clear();
			}
			else
			{
//				if (_state.renderTarget)
//				{
//					var sessions : Vector.<RenderSession> = _rtSessions[_state.renderTarget];
//					
//					sessions ||= _rtSessions[_state.renderTarget] = new Vector.<RenderSession>();
//					sessions.push(_currentSession);
//				}
//				else
//				{
//					_sessions[int(_numSessions++)] = _currentSession;
//				}
				
				if (_currentSession && _currentSession.renderState)
					_sessions[int(_numSessions++)] = _currentSession;
				
				_state = RENDER_STATE.create();
				_state.clear();
				_currentSession = RENDER_SESSION.create();
				_currentSession.renderState = _state;
				_currentSession.offsets.length = 0;
				_currentSession.numTriangles.length = 0;
			}
		}
		
	}
}