package aerys.minko.render.resource
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.Signature;
	import aerys.minko.type.Signal;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.display3D.Context3D;
	import flash.display3D.Program3D;
	import flash.utils.ByteArray;

	/**
	 * Program3DResource objects handle programs allocation and
	 * disposal using the Stage3D API.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class Program3DResource implements IResource
	{
		use namespace minko_render;
		
		private var _name						: String					= null;
		private var _numUses					: uint						= 0;
		private var _signature					: Signature					= null;
		private var _nativeProgram				: Program3D					= null;
		
		private var _vsProgram					: ByteArray					= null;
		private var _fsProgram					: ByteArray					= null;
		
		minko_render var _vertexInputComponents	: Vector.<VertexComponent> 	= null;
		minko_render var _vertexInputIndices	: Vector.<uint>				= null;
		minko_render var _vsConstants			: Vector.<Number>			= null;
		minko_render var _fsConstants			: Vector.<Number>			= null;
		minko_render var _fsTextures			: Vector.<ITextureResource>	= null;
		
		minko_render var _bindings				: Object					= null;
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get signature() : Signature
		{
			return _signature;
		}
		
		public function Program3DResource(name				: String,
										  signature			: Signature,
										  vsProgram			: ByteArray,
									   	  fsProgram			: ByteArray,
									   	  vertexComponents	: Vector.<VertexComponent>,
									      vertexIndices		: Vector.<uint>,
										  vsConstants		: Vector.<Number>,
										  fsConstants		: Vector.<Number>,
										  fsTextures		: Vector.<ITextureResource>,
										  bindings			: Object)
		{
			_name					= name;
			_signature				= signature;
			_vsProgram				= vsProgram;
			_fsProgram				= fsProgram;
			_vertexInputComponents	= vertexComponents;
			_vertexInputIndices		= vertexIndices;
			_vsConstants			= vsConstants;
			_fsConstants			= fsConstants;
			_fsTextures				= fsTextures;
			_bindings				= bindings;
		}
		
		public function retain() : void
		{
			++_numUses;
		}
		
		public function release() : void
		{
			--_numUses;
		}
		
		public function prepareContext(context 		: Context3DResource,
									   previous		: Program3DResource) : void
		{
			if (!_nativeProgram)
			{
				_nativeProgram = context.createProgram();
				_nativeProgram.upload(_vsProgram, _fsProgram);
			}
			
			if (previous !== this)
				context.setProgram(_nativeProgram);
		}
		
		public function dispose() : void
		{
			if (_nativeProgram)
				_nativeProgram.dispose();
		}
	}
}
