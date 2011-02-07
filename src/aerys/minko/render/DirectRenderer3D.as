package aerys.minko.render
{
	import aerys.minko.Viewport3D;
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.transform.TransformManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	import aerys.minko.type.vertex.format.NativeFormat;
	
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
		private var _states			: RenderStatesManager	= null;
		private var _transform		: TransformManager		= new TransformManager();
		private var _numTriangles	: uint					= 0;
		private var _viewport		: Viewport3D			= null;
		private var _drawingTime	: int					= 0;
		private var _emptyTexture	: TextureBase			= null;
		private var _frame			: uint					= 0;
		
		private var _stream			: VertexStream3D		= null;
		private var _format			: IVertex3DFormat		= null;
		private var _offset			: int					= 0;
		private var _tmp			: Vector.<Number>		= new Vector.<Number>();
		
		private var _textures		: Vector.<TextureBase>	= new Vector.<TextureBase>(8, true);
		
		public function get states() 		: RenderStatesManager	{ return _states; }
		public function get numTriangles()	: uint					{ return _numTriangles; }
		public function get viewport()		: Viewport3D			{ return _viewport; }
		public function get drawingTime()	: int					{ return _drawingTime; }
		public function get frameId()		: uint					{ return _frame; }
		
		public function DirectRenderer3D(viewport : Viewport3D, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_states = new RenderStatesManager();
			
			_emptyTexture = createTexture(1, 1);
		}

		public function setVertexStream(stream 	: VertexStream3D,
										offset	: int	= 0) : void
		{
			var format 	: IVertex3DFormat 	= states.vertexFormat || stream.format;
			
			if (_stream != stream || _format != format || _offset != offset)
			{
				var buffer	: VertexBuffer3D	= stream._nativeBuffer;
				
				if (!buffer)
				{
					buffer = _context.createVertexBuffer(stream.length,
														 stream.format.dwordsPerVertex);
					stream._nativeBuffer = buffer;
				}
				
				if (stream._update)
				{
					stream._update = false;
					buffer.uploadFromVector(stream._data, 0, stream.length);
					if (!stream._dynamic)
						stream._data = null;
				}
				
				var formats 	: Vector.<int> 	= format.nativeFormats;
				var numFormats 	: int 			= formats.length;
				var t			: int			= getTimer();
				var o 			: int 			= 0;
				
				// set input vertex streams
				for (var i : int = 0; i < numFormats; ++i)
				{
					var nativeFormatIndex : int = formats[i] - 1;
					
					_context.setVertexBufferAt(offset + i,
											   buffer,
											   o,
											   NativeFormat.STRINGS[nativeFormatIndex]);
					
					o += NativeFormat.NB_DWORDS[nativeFormatIndex];
				}
				
				_drawingTime += getTimer() - t;
				
				// disable the other streams
				while (i < 8)
					_context.setVertexBufferAt(i++, null);
				
				_stream = stream;
				_format = format;
				_offset = offset;
			}
		}
		
		public function drawTriangles(indexStream 	: IndexStream3D,
									  firstIndex	: uint	= 0,
									  count			: uint	= 0) : void
		{
			count ||= indexStream.length / 3;
			
			if (indexStream.length == 0 || count == 0)
				return ;
			
			//_context.enableErrorChecking = true;
			
			var t : int = getTimer();
			
			_states.apply(_context);
			_context.drawTriangles(indexStream.getIndexBuffer3D(_context),
								   firstIndex,
								   count);
			
			_drawingTime += getTimer() - t;
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
			
			var t : int = getTimer();
			
			_context.present();
			
			_drawingTime += getTimer() - t;
			
			_stream = null;
			_format = null;
			_offset = 0;
		}
		
		public function createTexture(width 	: uint,
							   		  height 	: uint,
							   		  format 	: String	= Context3DTextureFormat.BGRA,
							   		  optimized : Boolean 	= false) : TextureBase
		{
			return _context.createTexture(width, height, format, optimized);
		}
		
		public function beginRenderToTexture(texture 			: TextureBase,
								 			 depthAndStencil	: Boolean 	= false,
											 antiAliasing		: int		= 0,
										     surface			: int		= 0) : void
		{
			_context.setRenderToTexture(texture, depthAndStencil, antiAliasing, surface);
		}
		
		public function endRenderToTexture() : void
		{
			_context.setRenderToBackBuffer();
		}
		
		public function setMatrix(index			: int,
								  programType	: String,
								  matrix 		: Matrix4x4,
								  transposed	: Boolean	= true) : void
		{
			var t : int = getTimer();
			
			_context.setProgramConstantsFromMatrix(programType,
												   index,
												   matrix.matrix3D,
												   transposed);
			
			_drawingTime += getTimer() - t;
		}
		
		public function setConstants(firstRegister	: int,
									 programType	: String,
									 ...data) : void
		{
			var numConstants : int = data.length;
			
			_tmp.length = 0;
			for (var i : int = 0; i < numConstants; ++i)
				_tmp[i] = data[i];
			
			var t : int = getTimer();
			
			_context.setProgramConstantsFromVector(programType,
												   firstRegister,
												   _tmp);
			
			_drawingTime += getTimer() - t;
		}
		
		public function setTexture(index	: int,
								   texture	: TextureBase) : void
		{
			if (_textures[index] != texture)
			{
				_textures[index] = texture;
				
				var t : int = getTimer();
				
				_context.setTextureAt(index, texture);// || _emptyTexture);
				
				_drawingTime += getTimer() - t;
			}
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
		
		public function setScissorRectangle(rectangle : Rectangle) : void
		{
			_context.setScissorRectangle(rectangle);
		}
	}
}