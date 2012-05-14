package aerys.minko.render.shader
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.ns.minko_shader;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.Program3DResource;
	import aerys.minko.render.shader.compiler.graph.ShaderGraph;
	import aerys.minko.render.shader.compiler.graph.nodes.AbstractNode;
	import aerys.minko.render.shader.part.ShaderPart;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;
	
	import flash.utils.getQualifiedClassName;
	
	use namespace minko_shader;
	
	/**
	 * The base class to extend in order to create ActionScript shaders
	 * and program the GPU using AS3.
	 * 
	 * @author Jean-Marc Le Roux
	 * @author Romain Gilliotte
	 * 
	 * @see aerys.minko.render.shader.ShaderPart
	 * @see aerys.minko.render.shader.ShaderInstance
	 * @see aerys.minko.render.shader.ShaderSignature
	 * @see aerys.minko.render.shader.ShaderDataBindings
	 */
	public class Shader extends ShaderPart
	{
		use namespace minko_shader;
		use namespace minko_render;
		
		minko_shader var _meshBindings		: ShaderDataBindings			= null;
		minko_shader var _sceneBindings		: ShaderDataBindings			= null;
		minko_shader var _kills				: Vector.<AbstractNode>			= new <AbstractNode>[];
		
		private var _name					: String						= null;
		private var _enabled				: Boolean						= true;
		private var _defaultSettings		: ShaderSettings				= new ShaderSettings(null);
		
		private var _instances				: Vector.<ShaderInstance>		= new <ShaderInstance>[]
		private var _numActiveInstances		: uint							= 0;
		private var _numRenderedInstances	: uint							= 0;
		private var _settings				: Vector.<ShaderSettings>		= new <ShaderSettings>[];
		private var _programs				: Vector.<Program3DResource>	= new <Program3DResource>[];
		
		private var _begin					: Signal						= new Signal('Shader.begin');
		private var _end					: Signal						= new Signal('Shader.end');
		
		/**
		 * The name of the shader. Default value is the qualified name of the
		 * ActionScript shader class (example: "aerys.minko.render.effect.basic::BasicShader"). 
		 * 
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
		 * The signal executed when the shader (one of its forks) is used
		 * for rendering for the very first time during the current frame.
		 * Callbacks for this signal must accept the following arguments:
		 * <ul>
		 * <li>shader : Shader, the shader executing the signal</li>
		 * <li>context : Context3D, the context used for rendering</li>
		 * <li>backBuffer : RenderTarget, the render target used as the back-buffer</li>
		 * </ul>
		 * 
		 * @return 
		 * 
		 */
		public function get begin() : Signal
		{
			return _begin;
		}
		
		/**
		 * The signal executed when the shader (one of its forks) is used
		 * for rendering for the very last time during the current frame.
		 * Callbacks for this signal must accept the following arguments:
		 * <ul>
		 * <li>shader : Shader, the shader executing the signal</li>
		 * <li>context : Context3D, the context used for rendering</li>
		 * <li>backBuffer : RenderTarget, the render target used as the back-buffer</li>
		 * </ul>
		 * 
		 * @return 
		 * 
		 */
		public function get end() : Signal
		{
			return _end;
		}
		
		/**
		 * Whether the shader (and all its forks) are enabled for rendering
		 * or not.
		 * 
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
		}
		
		/**
		 *  
		 * @param priority Default value is 0.
		 * @param renderTarget Default value is null.
		 * 
		 */
		public function Shader(renderTarget	: RenderTarget	= null,
							   priority		: Number		= 0.0)
		{
			super(this);
			
			_defaultSettings.renderTarget = renderTarget;
			_defaultSettings.priority = priority;
			
			_name = getQualifiedClassName(this);
		}
		
		public function fork(meshBindings	: DataBindings,
							 sceneBindings	: DataBindings) : ShaderInstance
		{
			var pass : ShaderInstance = findPass(meshBindings, sceneBindings);
			
			if (pass == null)
			{
				var signature	: Signature			= new Signature(_name);
				var config		: ShaderSettings	= findOrCreateSettings(meshBindings, sceneBindings);

				signature.mergeWith(config.signature);
				
				var program		: Program3DResource	= null;
				
				//if (config.enabled)
				{
					program = findOrCreateProgram(meshBindings, sceneBindings);
					signature.mergeWith(program.signature);
				}
				
				pass = new ShaderInstance(this, config, program, signature);
				pass.retained.add(shaderInstanceRetainedHandler);
				pass.released.add(shaderInstanceReleasedHandler);
				
				_instances.push(pass);
			}
			
			return pass;
		}
		
		public function disposeUnusedResources() : void
		{
			var numInstances	: uint = _instances.length;
			var currentId		: uint = 0;
			
			for (var instanceId : uint = 0; instanceId < numInstances; ++instanceId)
			{
				var passInstance : ShaderInstance = _instances[instanceId];
				
				if (!passInstance.isDisposable)
					_instances[currentId++] = passInstance;
			}
			
			_instances.length = currentId;
		}
		
		/**
		 * Override this method to initialize the settings
		 * - such as the blending operands or the triangle culling - of the shader.
		 * This values can be read from both the mesh and the scene bindings.
		 *  
		 * @param settings
		 * 
		 * @see aerys.minko.render.shader.ShaderSettings
		 * 
		 */
		protected function initializeSettings(settings : ShaderSettings) : void
		{
//			throw new Error("The method 'initializeSettings' must be implemented.");
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
			throw new Error("The method 'getVertexPosition' must be implemented.");
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
			throw new Error("The method 'getPixelColor' must be implemented.");
		}
		
		
		private function findPass(meshBindings	: DataBindings,
								  sceneBindings	: DataBindings) : ShaderInstance
		{
			var numPasses : int = _instances.length;
			
			for (var passId : uint = 0; passId < numPasses; ++passId)
				if (_instances[passId].signature.isValid(meshBindings, sceneBindings))
					return _instances[passId];
			
			return null;
		}
		
		private function findOrCreateProgram(meshBindings	: DataBindings,
											 sceneBindings	: DataBindings) : Program3DResource
		{
			var numPrograms	: int = _programs.length;
			var program 	: Program3DResource;
			
			for (var programId : uint = 0; programId < numPrograms; ++programId)
				if (_programs[programId].signature.isValid(meshBindings, sceneBindings))
					return _programs[programId];
			
			var signature		: Signature		= new Signature(_name);
			
			_meshBindings	= new ShaderDataBindings(meshBindings, signature, Signature.SOURCE_MESH);
			_sceneBindings	= new ShaderDataBindings(sceneBindings, signature, Signature.SOURCE_SCENE);
			
			var vertexPosition	: AbstractNode	= getVertexPosition()._node;
			var pixelColor		: AbstractNode	= getPixelColor()._node;
			var shaderGraph		: ShaderGraph	= new ShaderGraph(vertexPosition, pixelColor, _kills);
			
			program			= shaderGraph.generateProgram(_name, signature);
			_meshBindings	= null;
			_sceneBindings	= null;
			_kills.length	= 0;
			
			_programs.push(program);
			
			return program;
		}
		
		private function findOrCreateSettings(meshBindings 	: DataBindings,
											 sceneBindings	: DataBindings) : ShaderSettings
		{
			var numConfigs	: int 				= _settings.length;
			var config		: ShaderSettings	= null;
			
			for (var configId : int = 0; configId < numConfigs; ++configId)
				if (_settings[configId].signature.isValid(meshBindings, sceneBindings))
					return _settings[configId];
			
			var signature : Signature = new Signature(_name);
			
			config			= _defaultSettings.clone(signature);
			_meshBindings	= new ShaderDataBindings(meshBindings, signature, Signature.SOURCE_MESH);
			_sceneBindings	= new ShaderDataBindings(sceneBindings, signature, Signature.SOURCE_SCENE);
			
			initializeSettings(config);
			
			_meshBindings	= null;
			_sceneBindings	= null;
			
			_settings.push(config);
			
			return config;
		}
		
		private function shaderInstanceRetainedHandler(instance : ShaderInstance,
													   numUses	: uint) : void
		{
			if (numUses == 1)
			{
				++_numActiveInstances;
				
				instance.begin.add(shaderInstanceBeginHandler);
				instance.end.add(shaderInstanceEndHandler);
			}
		}
		
		private function shaderInstanceReleasedHandler(instance : ShaderInstance,
													   numUses	: uint) : void
		{
			if (numUses == 0)
			{
				--_numActiveInstances;
				
				instance.begin.remove(shaderInstanceBeginHandler);
				instance.end.remove(shaderInstanceEndHandler);
			}
		}
		
		private function shaderInstanceBeginHandler(instance 	: ShaderInstance,
													context		: Context3DResource,
													backBuffer	: RenderTarget) : void
		{
			if (_numRenderedInstances == 0)
				_begin.execute(this, context, backBuffer);
		}
		
		private function shaderInstanceEndHandler(instance		: ShaderInstance,
												  context		: Context3DResource,
												  backBuffer	: RenderTarget) : void
		{
			_numRenderedInstances++;
			
			if (_numRenderedInstances == _numActiveInstances)
			{
				_numRenderedInstances = 0;
				_end.execute(this, context, backBuffer);
			}
		}
	}
}
