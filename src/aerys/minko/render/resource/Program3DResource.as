package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IParameterBinding;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;

	public final class Program3DResource implements IResource
	{
		use namespace minko_shader;

		private var _name				: String	= null;
		private var _update				: Boolean	= false;
		private var _nativeProgram		: Program3D	= null;
		
		private var _vsProgram			: ByteArray	= null;
		private var _fsProgram			: ByteArray	= null;
		
		minko_shader var _vertexComponents	: Vector.<VertexComponent> 		= null;
		minko_shader var _vertexIndices		: Vector.<uint>					= null;
		minko_shader var _vsConstants		: Vector.<Number>				= null;
		minko_shader var _fsConstants		: Vector.<Number>				= null;
		minko_shader var _fsTextures		: Vector.<ITextureResource>		= null;
		
		minko_shader var _bindings			: Vector.<IParameterBinding>	= null;
		
		public function get name() : String
		{
			return _name;
		}
		
		public function Program3DResource(name				: String,
										  vsProgram			: ByteArray,
									   	  fsProgram			: ByteArray,
									   	  vertexComponents	: Vector.<VertexComponent>,
									      vertexIndices		: Vector.<uint>,
										  vsConstants		: Vector.<Number>,
										  fsConstants		: Vector.<Number>,
										  fsTextures		: Vector.<ITextureResource>,
										  bindings			: Vector.<IParameterBinding>)
		{
			_name				= name;
			
			_vsProgram			= vsProgram;
			_fsProgram			= fsProgram;
			_vertexComponents	= vertexComponents;
			_vertexIndices		= vertexIndices;
			_vsConstants		= vsConstants;
			_fsConstants		= fsConstants;
			_fsTextures			= fsTextures;
			_bindings			= bindings;
		}

		public function getProgram3D(context : Context3D) : Program3D
		{
			if (!_nativeProgram)
			{
				_nativeProgram = context.createProgram();
				_nativeProgram.upload(_vsProgram, _fsProgram);
			}
			
			return _nativeProgram;
		}

		public function dispose() : void
		{
			if (_nativeProgram)
				_nativeProgram.dispose();
		}
	}
}
