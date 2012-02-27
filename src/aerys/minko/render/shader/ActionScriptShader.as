package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.RenderingList;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	
	import flash.display3D.Context3D;
	import flash.utils.getQualifiedClassName;
	
	use namespace minko_shader;
	
	public class ActionScriptShader extends ShaderPart
	{
		use namespace minko_shader;
		
		minko_shader var _meshBindings	: ShaderDataBindings	= null;
		minko_shader var _sceneBindings	: ShaderDataBindings	= null;
		minko_shader var _kills			: Vector.<INode>		= new <INode>[];
		
		private var _name				: String					= null;
		private var _enabled			: Boolean					= true;
		
		private var _forks			: Object					= {};
		private var _signatures			: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		
		private var _numPasses			: uint						= 0;
		private var _numActivePasses	: uint						= 0;
		
		private var _forked				: Signal					= new Signal();
		private var _begin				: Signal					= new Signal();
		private var _end				: Signal					= new Signal();
		
		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
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
			return _forked;
		}
		
		public function get begin() : Signal
		{
			return _begin;
		}
		
		public function get end() : Signal
		{
			return _end;
		}
		
		public function ActionScriptShader()
		{
			super(this);
			
			_name = getQualifiedClassName(this);
		}
		
		public function fork(meshBindings	: DataBindings,
							 sceneBindings	: DataBindings) : Shader
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
			
			var fork : Shader	= signId < numSignatures
				? _forks[signature.hash]
				: null;
			
			// no valid signature found
			if (!signature || !fork)
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
				
				fork = _forks[signature.hash];
				
				if (!fork)
				{
					// the signature really doesn't exist so we add it to the list
					_signatures.push(signature);
					
					Compiler.load(new ShaderGraph(op, oc, _kills), 0xffffffff);
					// compile the shader program
					fork = new Shader(
						Compiler.compileShader(_name),
						signature
					);
					initializeFork(fork);
					
					// store the new instance
					_forks[signature.hash] = fork;
					
					_forked.execute(this, fork);
				}
				
				_meshBindings = null;
				_sceneBindings = null;
			}
			
			return fork;
		}
		
		public function getForkBySignature(signature : ShaderSignature) : Shader
		{
			return _forks[signature.hash];
		}
		
		protected function initializeFork(fork : Shader) : void
		{
			fork.addedToRenderingList.add(
				shaderAddedToRenderingListHandler
			);
			fork.removedFromRenderingList.add(
				shaderRemovedFromRenderingListHandler
			);
			
			fork.begin.add(shaderBeginHandler);
			fork.end.add(shaderEndHandler);
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
		
		private function shaderAddedToRenderingListHandler(shader	: Shader,
														   list		: RenderingList) : void
		{
			++_numPasses;
		}
		
		private function shaderRemovedFromRenderingListHandler(shader	: Shader,
															   list		: RenderingList) : void
		{
			--_numPasses;
		}
		
		private function shaderBeginHandler(shader		: Shader,
											context		: Context3D,
											backbuffer	: RenderTarget,
											previous		: Shader) : void
		{
			if (_numActivePasses == 0)
				_begin.execute(this, context, backbuffer);
			
			++_numActivePasses;
		}
		
		private function shaderEndHandler(shader		: Shader,
										  context		: Context3D,
										  backbuffer	: RenderTarget,
										  previous		: Shader) : void
		{
			if (_numActivePasses == _numPasses)
			{
				_numActivePasses = 0;
				_end.execute(this, context, backbuffer);
			}
		}
	}
}
