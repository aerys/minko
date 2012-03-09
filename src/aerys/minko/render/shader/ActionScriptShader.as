package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.RenderingList;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.shader.compiler.Compiler;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.INode;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.enum.Blending;
	import aerys.minko.type.enum.TriangleCulling;
	
	import flash.display3D.Context3D;
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;
	
	use namespace minko_shader;
	
	/**
	 * The base class to extend in order to create ActionScript shaders.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 * @see aerys.minko.render.shader.Shader
	 * @see aerys.minko.render.shader.ShaderPart
	 * @see aerys.minko.render.shader.ShaderSignature
	 * àsee aerys.minko.render.shader.ShaderDataBindings
	 * 
	 */
	public class ActionScriptShader extends ShaderPart
	{
		use namespace minko_shader;
		
		minko_shader var _meshBindings	: ShaderDataBindings		= null;
		minko_shader var _sceneBindings	: ShaderDataBindings		= null;
		minko_shader var _kills			: Vector.<INode>			= new <INode>[];
		
		private var _name				: String					= null;
		private var _forks				: Object					= {};
		private var _enabled			: Boolean					= true;
		
		private var _shaderTemplate		: Shader					= new Shader(null);
		
		private var _signatures			: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		
		private var _numPasses			: uint						= 0;
		private var _numActivePasses	: uint						= 0;
		
		private var _forked				: Signal					= new Signal();
		private var _begin				: Signal					= new Signal();
		private var _end				: Signal					= new Signal();
		
		/**
		 * The name of the shader. Default value is the qualified name of the
		 * ActionScript shader class (example: "aerys.minko.render.effect.basic::BasicShader"). 
		 * @return 
		 * 
		 */
		public function get name() : String
		{
			return _name;
		}
		public function set name(value : String) : void
		{
			_name = value;
		}
		
		/**
		 * Whether the shader - and all its _forks - are enabled or not.
		 * Setting this property will also affect all the _forks of the ActionScript
		 * shader. 
		 * @return 
		 * 
		 */
		public function get enabled() : Boolean
		{
			return _enabled;
		}
		public function set enabled(value : Boolean) : void
		{
			_enabled = value;
			for each (var fork : Object in _forks)
				(fork as Shader).enabled = value;
		}

		/**
		 * The signal executed when a new fork is actually created. 
		 * @return 
		 * 
		 */
		public function get forked() : Signal
		{
			return _forked;
		}
		
		/**
		 * The signal executed when the very first fork created from
		 * this shader is being used for rendering. 
		 * @return 
		 * 
		 */
		public function get begin() : Signal
		{
			return _begin;
		}
		
		/**
		 * The signal executed when all the draw calls associated with any
		 * fork created from this shader have been executed. 
		 * @return 
		 * 
		 */
		public function get end() : Signal
		{
			return _end;
		}
		
		protected function get forkTemplate() : Shader
		{
			return _shaderTemplate;
		}
		
		/**
		 *  
		 * @param blending Default value is Blending.NORMAL.
		 * @param triangleCulling Default value is TriangleCulling.BACK.
		 * @param priority Default value is 0.
		 * @param renderTarget Default value is null.
		 * 
		 */
		public function ActionScriptShader(blending			: uint			= 524290,
										   triangleCulling	: uint			= 1,
										   priority			: Number		= 0.,
										   renderTarget		: RenderTarget	= null)
		{
			super(this);
			
			_shaderTemplate.blending = blending;
			_shaderTemplate.triangleCulling = triangleCulling;
			_shaderTemplate.priority = priority;
			_shaderTemplate.renderTarget = renderTarget;
			
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
					fork = _shaderTemplate.clone(
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
		
		/**
		 * Get a fork created from this shader by its signature. 
		 * @param signature
		 * @return 
		 * 
		 */
		public function getForkBySignature(signature : ShaderSignature) : Shader
		{
			return _forks[signature.hash];
		}
		
		private function initializeFork(fork : Shader) : void
		{
			fork.enabled = _enabled;
			
			fork.addedToRenderingList.add(
				shaderAddedToRenderingListHandler
			);
			fork.removedFromRenderingList.add(
				shaderRemovedFromRenderingListHandler
			);
			
			fork.program.drawCallCreated.add(drawCallCreatedHandler);
			
			fork.begin.add(shaderBeginHandler);
			fork.end.add(shaderEndHandler);
		}
		
		protected function drawCallCreatedHandler(program	: Program3DResource,
												  drawCall	: DrawCall) : void
		{
			initializeDrawCall(drawCall);
		}
		
		protected function initializeDrawCall(drawCall : DrawCall) : void
		{
			// nothing
		}
		
		/**
		 * The getVertexPosition() method is called to evaluate the vertex shader
		 * program that shall be executed on the GPU.
		 *  
		 * @return The position of the vertex in clip space (normalized screen space).
		 * 
		 */
		protected function getVertexPosition() : SFloat
		{
			throw new Error(
				"The method 'getVertexPosition' must be implemented."
			);
		}

		/**
		 * The getPixelColor() method is called to evaluate the fragment shader
		 * program that shall be executed on the GPU.
		 *  
		 * @return The color of the pixel on the screen.
		 * 
		 */
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
											previous	: Shader) : void
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
