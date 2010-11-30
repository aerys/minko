package aerys.minko.render
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.state.RenderStatesManager;
	import aerys.minko.render.transform.TransformManager;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3D;
	import aerys.minko.type.vertex.format.NativeFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
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
		
		private var _context		: Context3D					= null;
		private var _states			: RenderStatesManager		= null;
		private var _transform		: TransformManager			= new TransformManager();
		private var _numTriangles	: uint						= 0;
		private var _viewport		: Viewport3D				= null;
		private var _textures		: Vector.<TextureBase>		= new Vector.<TextureBase>(8, true);
		private var _drawingTime	: int						= 0;
		
		public function get states() 		: RenderStatesManager		{ return _states; }
		public function get transform()		: TransformManager			{ return _transform; }
		public function get numTriangles()	: uint						{ return _numTriangles; }
		public function get viewport()		: Viewport3D				{ return _viewport; }
		public function get textures()		: Vector.<TextureBase>		{ return _textures; }
		public function get drawingTime()	: int						{ return _drawingTime; }
		
		protected function get context() 	: Context3D					{ return _context; }
		
		public function NativeRenderer3D(myViewport : Viewport3D)
		{
			_viewport = myViewport;
			_context = myViewport.context;
			
			_states = new RenderStatesManager(_context);
			_transform.projection = _viewport.projection;
		}

		public function setVertexStream(stream	: VertexStream3D) : void
		{
			/*var formats 	: Vector.<int> 	= stream.format.nativeFormats;
			var numFormats 	: int 			= formats.length;
			var offset 		: int 			= 0;
			
			// set input vertex streams
			for (var i : int = 0; i < numFormats; ++i)
			{
				var nativeFormat : int = formats[i];
				
				_context.setVertexBufferAt(i,
										   stream._nativeBuffer,
										   offset,
										   NativeFormat.STRINGS[nativeFormat]);
				
				offset += NativeFormat.NB_DWORDS[nativeFormat];
			}
			
			// disable the other streams
			while (i < 8)
				_context.setVertexBufferAt(i++, null);*/
			//stream.
		}
		
		public function drawTriangles(indexStream 	: IndexStream3D,
									  firstIndex	: uint	= 0,
									  count			: uint	= 0) : void
		{
			count ||= indexStream.length / 3;
			_numTriangles += count;
			
			if (indexStream.length == 0 || indexStream.length / 3 < count)
				return ;
			
			var t : int = getTimer();
			
			_context.enableErrorChecking = true;
			_context.drawTriangles(indexStream._nativeBuffer,
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
	}
}