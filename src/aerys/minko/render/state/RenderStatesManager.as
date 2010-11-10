package aerys.minko.render.state
{
	import aerys.minko.ns.minko;
	import aerys.minko.type.AbstractStatesManager;
	import aerys.minko.render.shader.DefaultShader3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.type.vertex.formats.IVertex3DFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendMode;
	import flash.display3D.Context3DTriangleFace;
	import flash.display3D.TextureBase3D;

	public final class RenderStatesManager extends AbstractStatesManager
	{
		use namespace minko;
		
		private static const MAX_TEXTURES	: uint				= 8;
		
		private static const BLENDING_STR	: Vector.<String>	= Vector.<String>([Context3DBlendMode.DESTINATION_ALPHA,
																				   Context3DBlendMode.DESTINATION_COLOR,
																				   Context3DBlendMode.ONE,
																				   Context3DBlendMode.ONE_MINUS_DESTINATION_ALPHA,
																				   Context3DBlendMode.ONE_MINUS_DESTINATION_COLOR,
																				   Context3DBlendMode.ONE_MINUS_SOURCE_ALPHA,
																				   Context3DBlendMode.SOURCE_ALPHA,
																				   Context3DBlendMode.SOURCE_COLOR,
																				   Context3DBlendMode.ZERO]);
		
		private static const CULLING_STR	: Vector.<String>	= Vector.<String>([Context3DTriangleFace.NONE,
																				   Context3DTriangleFace.FRONT,
																				   Context3DTriangleFace.BACK]);
		
		public static const BLENDING			: uint			= RenderState.BLENDING;
		public static const SHADER				: uint			= RenderState.SHADER;
		public static const FORMAT				: uint			= RenderState.VERTEX_FORMAT;
		public static const WRITE_MASK			: uint			= RenderState.WRITE_MASK;
		public static const FRUSTUM_CULLING		: uint			= RenderState.FRUSTUM_CULLING;
		public static const TRIANGLE_CULLING	: uint			= RenderState.TRIANGLE_CULLING;
		public static const ALL					: uint			= RenderState.ALL;
		
		private var _context			: Context3D					= null;
		
		private var _blending			: uint						= 0;
		private var _shader				: Shader3D					= null;
		private var _format				: IVertex3DFormat			= null;
		private var _write				: uint						= 0;
		private var _frustumCulling		: uint						= 0;
		private var _triangleCulling	: uint						= 0;
		
		public function get shader() : Shader3D
		{
			return _shader;
		}
		
		public function get blending() : uint
		{
			return _blending;
		}
		
		public function get vertexFormat() : IVertex3DFormat
		{
			return _format;
		}
		
		public function set vertexFormat(value : IVertex3DFormat) : void
		{
			if (!(lockedStates & FORMAT))
				_format = value;
		}
		
		public function set shader(value : Shader3D) : void
		{
			if (!(lockedStates & SHADER))
			{
				_shader = value;
				_shader.prepareContext(_context);
			}
		}
		
		public function set blending(value : uint) : void
		{
			if (!(lockedStates & BLENDING))
			{
				_blending = value;
				_context.setBlending(BLENDING_STR[int(blending & 0xffff)],
									 BLENDING_STR[int(blending >> 16)]);
			}
		}
		
		public function get writeMask() : uint
		{
			return _write;
		}
		
		public function set writeMask(value : uint) : void
		{
			if (value != _write)
			{
				_write = value;
				_context.setColorWriteMask((_write & WriteMask.COLOR_RED) != 0,
										   (_write & WriteMask.COLOR_GREEN) != 0,
										   (_write & WriteMask.COLOR_BLUE) != 0,
										   (_write & WriteMask.COLOR_ALPHA) != 0);
			}
		}
		
		public function get frustumCulling() : uint
		{
			return _frustumCulling;
		}
		
		public function set frustumCulling(value : uint) : void
		{
			if (!(lockedStates & FRUSTUM_CULLING))
				_frustumCulling = value;
		}
		
		public function get triangleCulling() : uint
		{
			return _triangleCulling;
		}
		
		public function set triangleCulling(value : uint) : void
		{
			if (!(lockedStates & TRIANGLE_CULLING))
			{
				_triangleCulling = value;
				
				if ((_triangleCulling & TriangleCulling.FRONT)
					&& (_triangleCulling & TriangleCulling.BACK))
				{
					//_context.setCulling(Context3DTriangleFace.FRONT_AND_BACK);
				}
				else
				{
					trace("culling", CULLING_STR[_triangleCulling]);
					//_context.setCulling(CULLING_STR[_triangleCulling]);
				}
			}
		}
		
		public function RenderStatesManager(context : Context3D)
		{
			_context = context;
			
			blending = BlendingSource.DEFAULT | BlendingDestination.DEFAULT;
			shader = new DefaultShader3D();
			writeMask = WriteMask.ENABLED;
			_frustumCulling = FrustumCulling.ENABLED;
			triangleCulling = TriangleCulling.BACK;
			
			register(BLENDING, "blending", new Vector.<uint>());
			register(SHADER, "shader", new Vector.<Shader3D>());
			register(FORMAT, "vertexFormat", new Vector.<IVertex3DFormat>());
			register(WRITE_MASK, "writeMask", new Vector.<uint>());
			register(FRUSTUM_CULLING, "frustumCulling", new Vector.<uint>());
			register(TRIANGLE_CULLING, "triangleCulling", new Vector.<uint>());
		}
		
		public function compareTo(state : RenderStatesManager) : int
		{
			return 0;
		}
	}
}