package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	
	import flash.utils.getQualifiedClassName;
	
	use namespace minko_shader;
	
	public class Shader extends ShaderPart
	{
		use namespace minko_shader;
		
		minko_shader var _meshBindings	: ShaderDataBindings	= null;
		minko_shader var _sceneBindings	: ShaderDataBindings	= null;
		minko_shader var _kills			: Vector.<INode>		= new <INode>[];
		
		private var _name			: String					= null;
		
		private var _instances		: Object					= {};
		private var _signatures		: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		
		private var _instanciated	: Signal					= new Signal();
		private var _begin			: Signal					= new Signal();
		private var _end			: Signal					= new Signal();
		
		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}

		protected function get meshBindings() : ShaderDataBindings
		{
			return _meshBindings;
		}
		
		protected function get sceneBindings() : ShaderDataBindings
		{
			return _sceneBindings;
		}
		
		public function get instanciated() : Signal
		{
			return _instanciated;
		}
		
		public function get begin() : Signal
		{
			return _begin;
		}
		
		public function get end() : Signal
		{
			return _end;
		}
		
		public function Shader()
		{
			super(this);
			
			_name = getQualifiedClassName(this);
		}
		
		public function instanciate(meshBindings	: DataBindings,
									sceneBindings	: DataBindings) : ShaderInstance
		{
			// find compatible signature
			var numSignatures	: int 				= _signatures.length;
			var signature		: ShaderSignature	= null;
			
			for (var signId : int = 0; signId < numSignatures; ++signId)
			{
				signature = _signatures[signId];
				
				if (signature.isValid(meshBindings, sceneBindings))
					break ;
			}
			
			var instance : ShaderInstance	= signId < numSignatures
				? _instances[signature.hash]
				: null;
			
			// no valid signature found
			if (!signature || !instance)
			{
				signature = new ShaderSignature(this);
				_meshBindings = new ShaderDataBindings(
					meshBindings,
					signature,
					ShaderSignature.SOURCE_MESH
				);
				_sceneBindings = new ShaderDataBindings(
					sceneBindings,
					signature,
					ShaderSignature.SOURCE_SCENE
				);
				
				// generate the a new signature by evaluating the program
				var op	: INode = getVertexPosition()._node;
				var oc	: INode = getPixelColor()._node;
				
				instance = _instances[signature.hash];
				
				if (!instance)
				{
					// the signature really doesn't exist so we add it to the list
					_signatures.push(signature);
					
					// compile the shader program
					instance = new ShaderInstance(this, signature);
					initializeInstance(instance);
					
					Compiler.load(new ShaderGraph(op, oc, _kills), 0xffffffff);
					instance.program = Compiler.compileShader(_name);
					
					// store the new instance
					_instances[signature.hash] = instance;
					
					_instanciated.execute(this, instance);
				}
				
				_meshBindings = null;
				_sceneBindings = null;
			}
			
			return instance;
		}
		
		public function getInstanceBySignature(signature : ShaderSignature) : ShaderInstance
		{
			return _instances[signature.hash];
		}
		
		protected function initializeInstance(instance : ShaderInstance) : void
		{
			// nothing
		}
		
		protected function getVertexPosition() : SFloat
		{
			throw new Error(
				"The method 'getVertexPosition' must be implemented."
			);
		}

		protected function getPixelColor() : SFloat
		{
			throw new Error(
				"The method 'getVertexPosition' must be implemented."
			);
		}
	}
}
