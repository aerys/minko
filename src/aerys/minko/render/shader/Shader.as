package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	
	import flash.display3D.Program3D;
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;
	
	use namespace minko_shader;
	
	public class Shader extends ShaderPart
	{
		use namespace minko_shader;
		
		minko_shader var _localBindings		: ShaderDataBindings	= null;
		minko_shader var _globalBindings	: ShaderDataBindings	= null;
		minko_shader var _kills				: Vector.<INode>		= new <INode>[];
		
		private var _name			: String					= null;
		
		private var _instances		: Object					= {};
		private var _signatures		: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		
		private var _instanciated	: Signal					= new Signal();
		
		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function get instanciated() : Signal
		{
			return _instanciated;
		}

		protected function get localBindings() : ShaderDataBindings
		{
			return _localBindings;
		}
		
		protected function get globalBindings() : ShaderDataBindings
		{
			return _globalBindings;
		}
		
		public function Shader()
		{
			super(this);
			
			_name = getQualifiedClassName(this);
		}
		
		public function instanciate(localBindings	: DataBindings,
									globalBindings	: DataBindings) : ShaderInstance
		{
			// find compatible signature
			var numSignatures	: int 				= _signatures.length;
			var signature		: ShaderSignature	= null;
			
			for (var signId : int = 0; signId < numSignatures; ++signId)
			{
				signature = _signatures[signId];
				
				if (signature.isValid(localBindings, globalBindings))
					break ;
			}
			
			var instance : ShaderInstance	= signId < numSignatures
				? _instances[signature.hash]
				: null;
			
			// no valid signature found
			if (!signature || !instance)
			{
				signature = new ShaderSignature();
				_localBindings = new ShaderDataBindings(
					localBindings,
					signature,
					ShaderSignature.SOURCE_LOCAL
				);
				_globalBindings = new ShaderDataBindings(
					localBindings,
					signature,
					ShaderSignature.SOURCE_GLOBAL
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
					
					Compiler.load(new ShaderGraph(op, oc, _kills), 0xffffffff);
					instance.program = Compiler.compileShader(_name);
					
					// store the new instance
					_instances[signature.hash] = instance;
					
					_instanciated.execute(this, instance);
				}
				
				_localBindings = null;
				_globalBindings = null;
			}
			
			return instance;
		}
		
		private function addSignature(signature 		: ShaderSignature,
									  localBindings		: DataBindings,
									  globalBindings	: DataBindings) : void
		{
			_signatures.push(signature);
			
			var numKeys	: uint	= signature.numKeys;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String		= signature.getKey(i);
				var flags	: uint			= signature.getFlags(i);
				
				if (flags & ShaderSignature.SOURCE_GLOBAL)
				{
					localBindings.getPropertyChangedSignal(key).add(
						localPropertyChangedHandler
					);
				}
				else
				{
					globalBindings.getPropertyChangedSignal(key).add(
						globalPropertyChangedHandler
					);
				}
			}
		}
		
		private function localPropertyChangedHandler(dataBindings	: DataBindings,
													 propertyName	: String,
													 oldValue		: Object,
													 newValue		: Object) : void
		{
			var numSignatures	: uint	= _signatures.length;
			
			for (var i : uint = 0; i < numSignatures; ++i)
			{
				(_signatures[i] as ShaderSignature).checkProperty(
					propertyName,
					newValue,
					ShaderSignature.SOURCE_LOCAL
				);
			}
		}
		
		private function globalPropertyChangedHandler(dataBindings	: DataBindings,
													  propertyName	: String,
													  oldValue		: Object,
													  newValue		: Object) : void
		{
			var numSignatures	: uint	= _signatures.length;
			
			for (var i : uint = 0; i < numSignatures; ++i)
			{
				(_signatures[i] as ShaderSignature).checkProperty(
					propertyName,
					newValue,
					ShaderSignature.SOURCE_GLOBAL
				);
			}
		}
		
		protected function getVertexPosition() : SFloat
		{
			throw new Error("The method 'getVertexPosition' must be implemented.");
		}

		protected function getPixelColor() : SFloat
		{
			throw new Error("The method 'getVertexPosition' must be implemented.");
		}
	}
}
