package aerys.minko.render.shader.compiler
{
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.resource.texture.ITextureResource;
	import aerys.minko.render.shader.binding.IParameterBinding;
	
	import flash.utils.Dictionary;
	
	public class ShaderProgram
	{
		private var _bindings			: Vector.<IParameterBinding>;
		private var _bindingsByName		: Object;
		
		private var _program			: Program3DResource;
		private var _name				: String;
		
		private var _vsConstData		: Vector.<Number>;
		private var _fsConstData		: Vector.<Number>;
		private var _textures			: Vector.<ITextureResource>;
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get program() : Program3DResource
		{
			return _program;
		}
		
		public function get vertexShaderConstants() : Vector.<Number>
		{
			return _vsConstData;
		}
		
		public function get fragmentShaderConstants() : Vector.<Number>
		{
			return _fsConstData;
		}
		
		public function get bindings() : Vector.<IParameterBinding>
		{
			return _bindings;
		}
		
		public function ShaderProgram(name			: String,
									  program		: Program3DResource,
									  vsConstData	: Vector.<Number>,
									  fsConstData	: Vector.<Number>,
									  textures		: Vector.<ITextureResource>,
									  bindings		: Vector.<IParameterBinding>)
		{
			_name			= name;
			_program		= program;
			_bindings		= bindings;
			_bindingsByName	= new Dictionary();
			
			_vsConstData	= vsConstData;
			_fsConstData	= fsConstData;
			_textures		= textures;
			
			initialize();
		}
		
		private function initialize() : void
		{
			var numBindings : uint = _bindings.length;
			
			for (var bindingId : uint = 0; bindingId < numBindings; ++bindingId)
			{
				var binding : IParameterBinding = _bindings[bindingId];
				
				_bindingsByName[binding.name] = binding;
			}
		}
		
		/*public function fillRenderState(state			: RendererState, 
										styleData		: StyleData, 
										transformData	: TransformData, 
										worldData		: Dictionary) : void
		{
			// update constant data and textures
			var numBindings : uint = _bindings.length;
			for (var bindingId : uint = 0; bindingId < numBindings; ++bindingId)
				_bindings[bindingId].update(_vsConstData, _fsConstData, _textures, styleData, transformData, worldData);
			
			// fill state
			state.setVertexConstants(_vsConstData);
			state.setFragmentConstants(_fsConstData);
			state.setTextures(_textures);
			state.program = _program;
		}*/
		
		public function setNamedParameter(key		: String,
										  newValue	: Object) : void
		{
			IParameterBinding(_bindingsByName[key]).set(_vsConstData, _fsConstData, _textures, newValue);
		}
		
		public function dispose():void
		{
			_program.dispose();
			_program	= null;
			_bindings	= null;
			_name		+= '(disposed)';
		}
	}
}
