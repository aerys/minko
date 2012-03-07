package aerys.minko.render.resource
{
	import aerys.minko.ns.*;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IBinder;
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
		private var _name				: String					= null;
		private var _update				: Boolean					= false;
		private var _nativeProgram		: Program3D					= null;
		
		private var _vsProgram			: ByteArray					= null;
		private var _fsProgram			: ByteArray					= null;
		
		private var _vertexComponents	: Vector.<VertexComponent> 	= null;
		private var _vertexIndices		: Vector.<uint>				= null;
		private var _vsConstants		: Vector.<Number>			= null;
		private var _fsConstants		: Vector.<Number>			= null;
		private var _fsTextures			: Vector.<ITextureResource>	= null;
		
		private var _bindings			: Object					= null;
		
		private var _drawCallCreated	: Signal					= new Signal();
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get drawCallCreated() : Signal
		{
			return _drawCallCreated;
		}
		
		public function Program3DResource(name				: String,
										  vsProgram			: ByteArray,
									   	  fsProgram			: ByteArray,
									   	  vertexComponents	: Vector.<VertexComponent>,
									      vertexIndices		: Vector.<uint>,
										  vsConstants		: Vector.<Number>,
										  fsConstants		: Vector.<Number>,
										  fsTextures		: Vector.<ITextureResource>,
										  bindings			: Object)
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
		
		public function createDrawCall() : DrawCall
		{
			var drawCall : DrawCall = new DrawCall(
				_vsConstants,
				_fsConstants,
				_fsTextures,
				_vertexComponents,
				_vertexIndices,
				_bindings
			);
			
			_drawCallCreated.execute(this, drawCall);
			
			return drawCall;
		}

		public function dispose() : void
		{
			if (_nativeProgram)
				_nativeProgram.dispose();
		}
	}
}
