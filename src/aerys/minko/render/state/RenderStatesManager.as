package aerys.minko.render.state
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.shader.DefaultShader3D;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	import aerys.minko.type.vertex.format.Vertex3DFormat;
	
	import flash.display3D.Context3D;
	import flash.display3D.Context3DBlendFactor;
	import flash.display3D.Context3DTriangleFace;

	public final class RenderStatesManager
	{
		use namespace minko;
		
		private static const BLENDING_STR	: Vector.<String>	= Vector.<String>([Context3DBlendFactor.DESTINATION_ALPHA,
																				   Context3DBlendFactor.DESTINATION_COLOR,
																				   Context3DBlendFactor.ONE,
																				   Context3DBlendFactor.ONE_MINUS_DESTINATION_ALPHA,
																				   Context3DBlendFactor.ONE_MINUS_DESTINATION_COLOR,
																				   Context3DBlendFactor.ONE_MINUS_SOURCE_ALPHA,
																				   Context3DBlendFactor.SOURCE_ALPHA,
																				   Context3DBlendFactor.SOURCE_COLOR,
																				   Context3DBlendFactor.ZERO]);
		
		private static const CULLING_STR	: Vector.<String>	= Vector.<String>([Context3DTriangleFace.NONE,
																				   Context3DTriangleFace.BACK,
																				   Context3DTriangleFace.FRONT]);
		
		public static const BLENDING			: uint	= RenderState.BLENDING;
		public static const SHADER				: uint	= RenderState.SHADER;
		public static const FORMAT				: uint	= RenderState.VERTEX_FORMAT;
		public static const WRITE_MASK			: uint	= RenderState.WRITE_MASK;
		public static const TRIANGLE_CULLING	: uint	= RenderState.TRIANGLE_CULLING;
		public static const ALL					: uint	= RenderState.ALL;
		
		private var _set		: RenderState	= new RenderState();
		private var _popFlags	: uint				= 0;
		
		public function get shader() : Shader3D
		{
			return _set.shader;
		}
		
		public function get blending() : uint
		{
			return _set.blending;
		}
		
		public function get vertexFormat() : IVertex3DFormat
		{
			return _set.vertexFormat;
		}
		
		public function get writeMask() : uint
		{
			return _set.writeMask;
		}
		
		public function get triangleCulling() : uint
		{
			return _set.triangleCulling;
		}
		
		public function set vertexFormat(value : IVertex3DFormat) : void
		{
			_set.vertexFormat = value;
		}
		
		public function set shader(value : Shader3D) : void
		{
			_set.shader = value;
		}
		
		public function set blending(value : uint) : void
		{
			_set.blending = value;
		}
		
		public function set writeMask(value : uint) : void
		{
			_set.writeMask = value;
		}
		
		public function set triangleCulling(value : uint) : void
		{
			_set.triangleCulling = value;
		}
		
		public function RenderStatesManager()
		{
			blending = Blending.NORMAL;
			shader = new DefaultShader3D();
			writeMask = WriteMask.ALL;
			triangleCulling = TriangleCulling.BACK;
			vertexFormat = null;//Vertex3DFormat.XYZ_UV;
		}
		
		public function push() : void
		{
			_set = RenderState.push(_set);
		}
		
		public function pop() : void
		{
			_popFlags |= _set.setFlags;
			_set = RenderState.pop(_set);
		}
		
		public function apply(context : Context3D) : void
		{
			var flags : uint = _popFlags | _set.setFlags;
			
			_popFlags = 0;
			
			if (flags & SHADER)
				_set.shader.prepare(context);
			
			if (flags & TRIANGLE_CULLING)
			{
				var culling : uint = _set.triangleCulling;
				
				if ((culling & TriangleCulling.FRONT)
					&& (culling & TriangleCulling.BACK))
				{
					context.setCulling(Context3DTriangleFace.FRONT_AND_BACK);
				}
				else
				{
					context.setCulling(CULLING_STR[culling]);
				}
			}
			
			if (flags & WRITE_MASK)
			{
				var mask : uint = _set.writeMask;
				
				context.setColorMask((mask & WriteMask.COLOR_RED) != 0,
									 (mask & WriteMask.COLOR_GREEN) != 0,
									 (mask & WriteMask.COLOR_BLUE) != 0,
									 (mask & WriteMask.COLOR_ALPHA) != 0);
			}
			
			if (flags & BLENDING)
			{
				var blending : uint = _set.blending;
				
				context.setBlendFactors(BLENDING_STR[int(blending & 0xffff)],
										BLENDING_STR[int(blending >> 16)]);
			}
		}
	}
}