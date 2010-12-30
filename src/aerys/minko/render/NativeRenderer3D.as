package aerys.minko.render
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	import aerys.minko.type.vertex.format.NativeFormat;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.VertexBuffer3D;
	import flash.display3D.textures.Texture;
	import flash.display3D.textures.TextureBase;
	import flash.utils.getTimer;

	public class NativeRenderer3D implements IRenderer3D
	{
		use namespace minko;
		
		public static const BLENDING_STR	: Vector.<String>	= Vector.<String>([Context3DBlendFactor.DESTINATION_ALPHA,
																				   Context3DBlendFactor.DESTINATION_COLOR,
																				   Context3DBlendFactor.ONE,
																				   Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA,
																				   Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR,
																				   Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA,
																				   Context3DBlendFactor.SOURCE_ALPHA,
																				   Context3DBlendFactor.SOURCE_COLOR,
																				   Context3DBlendFactor.ZERO]);
		
		private var _context		: Context3D				= null;
		private var _states			: RenderStatesManager	= null;
		private var _transform		: TransformManager		= new TransformManager();
		private var _numTriangles	: uint					= 0;
		private var _viewport		: Viewport3D			= null;
		private var _drawingTime	: int					= 0;
		
		public function get states() 		: RenderStatesManager	{ return _states; }
		public function get transform()		: TransformManager		{ return _transform; }
		public function get numTriangles()	: uint					{ return _numTriangles; }
		public function get viewport()		: Viewport3D			{ return _viewport; }
		public function get drawingTime()	: int					{ return _drawingTime; }
		
		public function NativeRenderer3D(viewport : Viewport3D, context : Context3D)
		{
			_viewport = viewport;
			_context = context;
			
			_states = new RenderStatesManager(_context);
		}

		public function setVertexStream(stream 	: VertexStream3D,
										offset	: int	= 0) : void
		{
			var numVertices : int 				= stream.length;
			var format 		: IVertex3DFormat 	= states.vertexFormat || stream.format;
			var buffer		: VertexBuffer3D	= stream._nativeBuffer;
			
			if (!buffer)
			{
				buffer = _context.createVertexBuffer(numVertices,
													 stream.format.dwordsPerVertex);
				stream._nativeBuffer = buffer;
			}
			
			if (stream._update)
			{
				stream._update = false;
				buffer.uploadFromVector(stream._data, 0, numVertices);
			}
			
			var formats 	: Vector.<int> 	= format.nativeFormats;
			var numFormats 	: int 			= formats.length;
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
			
			// disable the other streams
			while (i < 8)
				_context.setVertexBufferAt(i++, null);
		}
		
		protected function prepareContext() : void
		{
			_context.enableErrorChecking = true;
		}
		
		public function drawTriangles(indexStream 	: IndexStream3D,
									  firstIndex	: uint	= 0,
									  count			: uint	= 0) : void
		{
			count ||= indexStream.length / 3;
			_numTriangles += count;
			
			if (indexStream.length == 0 || count == 0)
				return ;
			
			prepareContext();
			
			var t : int = getTimer();
			
			_context.drawTriangles(indexStream.getIndexBuffer3D(_context),
								   firstIndex,
								   count);
			
			_drawingTime += getTimer() - t;
		}
		
		public function clear(color : uint = 0xff000000) : void
		{
			_context.clear(((color >> 16) & 0xff) / 255.,
						   ((color >> 8) & 0xff) / 255.,
						   (color & 0xff) / 255.,
						   ((color >> 24) & 0xff) / 255.);
			_numTriangles = 0;
			_drawingTime = 0;
		}
		
		public function present() : void
		{
			_context.present();
		}
		
		public function createTexture(width 	: uint,
							   		  height 	: uint,
							   		  format 	: String	= Context3DTextureFormat.BGRA,
							   		  optimized : Boolean 	= false) : Texture
		{
			return _context.createTexture(width, height, format, optimized);
		}
		
		public function beginRenderToTexture(texture 			: TextureBase,
								 			 depthAndStencil	: Boolean = true) : void
		{
			_context.setRenderToTexture(texture, depthAndStencil);
		}
		
		public function endRenderToTexture() : void
		{
			_context.setRenderToBackBuffer();
		}
		
		public function setMatrix(index			: int,
								  matrix 		: Matrix4x4,
								  programType	: String,
								  transposed	: Boolean	= true) : void
		{
			_context.setProgramConstantsFromMatrix(programType,
												   index,
												   matrix.matrix3D,
												   transposed);
		}
		
		public function setConstants(firstRegister	: int,
									 data			: Vector.<Number>,
									 programType	: String,
									 numRegisters	: int = -1) : void
		{
			_context.setProgramConstantsFromVector(programType,
												   firstRegister,
												   data,
												   numRegisters);
		}
		
		public function setTexture(index	: int,
								   texture	: TextureBase) : void
		{
			_context.setTextureAt(index, texture);
		}
		
		public function drawToBitmapData(bitmapData : BitmapData) : void
		{
			_context.drawToBitmapData(bitmapData);
		}
	}
}