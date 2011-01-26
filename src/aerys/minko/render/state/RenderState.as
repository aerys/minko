package aerys.minko.render.state
{
	import aerys.common.Factory;
	import aerys.minko.render.shader.Shader3D;
	import aerys.minko.type.vertex.format.IVertex3DFormat;
	
	import flash.display3D.Context3D;

	public final class RenderState
	{
		public static const BLENDING			: uint	= 1;
		public static const SHADER				: uint	= 2;
		public static const VERTEX_FORMAT		: uint	= 4;
		public static const WRITE_MASK			: uint	= 8;
		public static const TRIANGLE_CULLING	: uint	= 16;
		public static const ALL					: uint	= BLENDING | SHADER
														  | VERTEX_FORMAT
														  | WRITE_MASK
														  | TRIANGLE_CULLING;
		
		private static const FACTORY	: Factory			= Factory.getFactory(RenderState);
		
		private var _target				: RenderState	= null;
		private var _setFlags			: uint	= 0;
		
		private var _blending			: uint				= 0;
		private var _shader				: Shader3D			= null;
		private var _format				: IVertex3DFormat	= null;
		private var _write				: uint				= 0;
		private var _triangleCulling	: uint				= 0;
		
		public function get setFlags() : uint
		{
			return _setFlags | (_target ? _target.setFlags : 0);
		}
		
		public function get writeMask() : uint
		{
			return _setFlags & WRITE_MASK
				   ? _write
				   : _target.writeMask; 
		}
		
		public function get shader() : Shader3D
		{
			return _setFlags & SHADER
				   ? _shader
				   : _target.shader;
		}
		
		public function get blending() : uint
		{
			return _setFlags & BLENDING
				   ? _blending
				   : _target.blending;
		}
		
		public function get vertexFormat() : IVertex3DFormat
		{
			return _setFlags & VERTEX_FORMAT
				   ? _format
				   : _target.vertexFormat;
		}
		
		public function get triangleCulling() : uint
		{
			return _setFlags & TRIANGLE_CULLING
				   ? _triangleCulling
				   : _target.triangleCulling;
		}
		
		public function set setFlags(value : uint) : void
		{
			_setFlags = value;
		}
		
		public function set vertexFormat(value : IVertex3DFormat) : void
		{
			_format = value;
			_setFlags |= VERTEX_FORMAT;
		}
		
		public function set shader(value : Shader3D) : void
		{
			//if (_shader != value)
			{
				_setFlags |= SHADER;
				_shader = value;
			}
		}
		
		public function set blending(value : uint) : void
		{
			//if (_blending != value)
			{
				_setFlags |= BLENDING;
				_blending = value;
			}
		}
		
		public function set writeMask(value : uint) : void
		{
			//if (value != _write)
			{
				_setFlags |= WRITE_MASK;
				_write = value;
			}
		}
		
		public function set triangleCulling(value : uint) : void
		{
			//if (value != _triangleCulling)
			{
				_setFlags |= TRIANGLE_CULLING;
				_triangleCulling = value;
			}
		}
		
		public static function push(states : RenderState) : RenderState
		{
			var set : RenderState	= FACTORY.create();
			
			set._target = states;
			set._setFlags = 0;
			
			return set;
		}
		
		public static function pop(states : RenderState) : RenderState
		{
			var set : RenderState = states._target;
			
			FACTORY.free(states);
			
			return set;
		}
		
	}
}