package aerys.minko.render
{
	import aerys.minko.Viewport3D;
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.Shader3D;
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
		
		private var _context		: Context3D				= null;
		private var _state			: RenderState			= new RenderState();
		private var _transform		: TransformManager		= new TransformManager();
		private var _numTriangles	: uint					= 0;
		private var _viewport		: Viewport3D			= null;
		private var _drawingTime	: int					= 0;
		private var _emptyTexture	: TextureBase			= null;
		private var _frame			: uint					= 0;
		
		private var _session		: RenderSession			= null;
		
		public function get state() 		: RenderState	{ return _state; }
		public function get numTriangles()	: uint			{ return _numTriangles; }
		public function get viewport()		: Viewport3D	{ return _viewport; }
		public function get drawingTime()	: int			{ return _drawingTime; }
		public function get frameId()		: uint			{ return _frame; }
		
		public function DirectRenderer3D(viewport : Viewport3D, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_emptyTexture = createTexture(1, 1);
		}

		public function drawTriangles(firstIndex	: uint	= 0,
									  count			: uint	= 0) : void
		{
			var indexStream:IndexStream3D = state.indexStream; 
			
			count ||= indexStream.length / 3;
			
			if (indexStream.length == 0 || count == 0)
				return ;
			
			//_context.enableErrorChecking = true;
			
			/*if (!_session || _session.renderState.version != _states.version)
			{
				var session : RenderSession = new RenderSession();
				
				session.next = _session;
				_session = session;
				_states.copy(_session.renderState);
			}
			
			var dc : DrawCall = new DrawCall();
			
			dc.initialize(_stream, indexStream, firstIndex, count);
			_session.drawCalls.push(dc);*/
			
			var t : int = getTimer();
			
			_state.prepareContext(_context);
				
			_context.drawTriangles(indexStream.getIndexBuffer3D(_context),
								   firstIndex,
								   count);
			
			_drawingTime += getTimer() - t;
			
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
			++_frame;
			
			/*var t : int = getTimer();
			var oldStates : RenderState = new RenderState();
			
			while (_session)
			{
				var states : RenderState = _session.renderState;
				var drawCalls	: Vector.<DrawCall> = _session.drawCalls;
				var numDrawCalls : int = drawCalls.length;
				
				states.prepareContext(oldStates, _context);
				for (var i : int = 0; i < numDrawCalls; ++i)
				{
					var drawCall : DrawCall = drawCalls[i];
					
					setVertexStream(drawCall.vertexBuffer);
					_context.drawTriangles(drawCall.indexBuffer.getIndexBuffer3D(_context),
										   drawCall.offset,
										   drawCall.numTriangles);
				}
				
				oldStates = states;
				_session = _session.next;
			}
			*/
			
			_context.present();
			
			//_drawingTime += getTimer() - t;
			
//			ligne 38
//			_stream = null;
//			_format = null;
//			_offset = 0;
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