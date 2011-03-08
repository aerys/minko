package aerys.minko.render
{
	import aerys.common.Factory;
	import aerys.minko.Viewport3D;
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	import aerys.minko.type.vertex.format.NativeFormat;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.TextureBase;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	import flash.utils.getTimer;

	public class DirectRenderer3D implements IRenderer3D
	{
		use namespace minko;
		
		private static const RENDER_SESSION	: Factory			= Factory.getFactory(RenderSession);
		private static const RENDER_STATE	: Factory			= Factory.getFactory(RenderState);
		
		private static const DIRECT			: Boolean			= false;
		
		private var _context		: Context3D					= null;
		private var _state			: RenderState				= new RenderState();
		private var _currentState	: RenderState				= new RenderState();
		private var _transform		: TransformManager			= new TransformManager();
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport3D				= null;
		private var _drawingTime	: int						= 0;
		private var _frame			: uint						= 0;
		
		private var _sessions		: Vector.<RenderSession>	= new Vector.<RenderSession>();
		private var _numSessions	: int						= 0;
		
		public function get state() 		: RenderState	{ return _state; }
		public function get numTriangles()	: uint			{ return _numTriangles; }
		public function get viewport()		: Viewport3D	{ return _viewport; }
		public function get drawingTime()	: int			{ return _drawingTime; }
		public function get frameId()		: uint			{ return _frame; }
		
		public function DirectRenderer3D(viewport : Viewport3D, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			//_context.enableErrorChecking = true;
		}

		public function drawTriangles(firstIndex	: uint	= 0,
									  count			: uint	= 0) : void
		{
			var indexStream : IndexStream3D = _state.indexStream; 
			
			count ||= indexStream.length / 3;
			
			if (indexStream.length == 0 || count == 0)
				return ;
		
			if (!DIRECT)
			{
				var session : RenderSession = RENDER_SESSION.create();
				
				session.renderState = _state;
				session.offset = firstIndex;
				session.numTriangles = count;
				
				_sessions[int(_numSessions++)] = session;
				
				_state = RENDER_STATE.create();
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
			
			_state.clear();
			_numTriangles += count;
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
		}
		
		public function present() : void
		{
			var time : int = getTimer();
			
			for (var i : int = 0; i < _numSessions; ++i)
			{
				var session : RenderSession = _sessions[i];
				var state	: RenderState 	= session.renderState;
				
				state.prepareContext(_context, _currentState);
				_context.drawTriangles(state.indexStream.getIndexBuffer3D(_context),
									   session.offset,
									   session.numTriangles);

				RENDER_SESSION.free(session);
				RENDER_STATE.free(_currentState);
				
				_currentState = state;
			}
			
			_numSessions = 0;
			_context.present();
			
			_drawingTime += getTimer() - time;
			++_frame;
		}
		
		public function createTexture(width 	: uint,
							   		  height 	: uint,
							   		  format 	: String	= Context3DTextureFormat.BGRA,
							   		  optimized : Boolean 	= false) : TextureBase
		{
			return _context.createTexture(width, height, format, optimized);
		}
		

		public function drawToBitmapData(bitmapData : BitmapData) : void
		{
			_context.drawToBitmapData(bitmapData);
		}
		
		public function createShader(vertexShader 	: ByteArray,
									 fragmentShader	: ByteArray) : Shader3D
		{
			// FIXME
			return null;
		}
		
	}
}