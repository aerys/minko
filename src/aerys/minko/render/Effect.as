package aerys.minko.render
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataBindingsProxy;
	import aerys.minko.type.binding.Signature;

	/**
	 * The base class to define effects.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Effect
	{
		private var _extraPasses	: Vector.<Shader>;
		private var _instances		: Vector.<EffectInstance>;
		
		public function get numExtraPasses() : uint
		{
			return _extraPasses.length;
		}
		
		public function Effect(...extraPasses)
		{
			initialize(extraPasses);
		}
		
		private function initialize(passes : Array) : void
		{
			while (passes[0] is Array)
				passes = passes[0];
			
			_extraPasses = Vector.<Shader>(passes);
			_instances = new <EffectInstance>[];
		}
		
		public function getExtraPass(index : uint = 0) : Shader
		{
			return _extraPasses[index];
		}
		
		public function setExtraPasses(extraPasses : Vector.<Shader>) : Effect
		{
			var numExtraPasses : uint = extraPasses.length;
			
			for (var extraPassId : uint = 0; extraPassId < numExtraPasses; ++extraPassId)
				_extraPasses[extraPassId] = extraPasses[extraPassId];
			_extraPasses.length = numExtraPasses;
			
			invalidateInstance();
			
			return this;
		}
		
		public function addExtraPass(pass : Shader) : Effect
		{
			if (hasExtraPass(pass))
				throw new Error('This pass is already in the effect.');
			
			_extraPasses.push(pass);
			
			invalidateInstance();
			
			return this;
		}
		
		public function removeExtraPass(pass : Shader) : Effect
		{
			var numPasses 	: int 	= _extraPasses.length - 1;
			var index		: int	= _extraPasses.indexOf(pass);
			
			if (index < 0)
				throw new Error('This pass does not exists.');
			
			for (; index < numPasses; index++)
				_extraPasses[index] = _extraPasses[int(index + 1)];
			
			_extraPasses.length = numPasses;
			
			invalidateInstance();
			
			return this;
		}
		
		public function hasExtraPass(pass : Shader) : Boolean
		{
			return _extraPasses.indexOf(pass) >= 0;
		}
		
		public function clone() : Effect
		{
			return new Effect().setExtraPasses(_extraPasses);
		}
		
		private function invalidateInstance() : void
		{
			var numInstances : uint = _instances.length;
			
			for (var instanceId : uint = 0; instanceId < numInstances; ++instanceId)
			{
				var instance : EffectInstance = _instances[instanceId] as EffectInstance;
				
				instance.passesChanged.execute(instance);
			}
		}
		
		public function fork(sceneBindings	: DataBindings,
							 meshBindings 	: DataBindings) : EffectInstance
		{
			var instance : EffectInstance = findInstance(sceneBindings, meshBindings);
			
			if (!instance)
			{
				var signature : Signature = new Signature();
				var sceneBindingsProxy : DataBindingsProxy	= new DataBindingsProxy(
					sceneBindings, signature, Signature.SOURCE_SCENE
				);
				var meshBindingsProxy : DataBindingsProxy	= new DataBindingsProxy(
					meshBindings, signature, Signature.SOURCE_MESH
				);
				
				instance = new EffectInstance(
					this,
					initializePasses(sceneBindingsProxy, meshBindingsProxy),
					_extraPasses,
					signature
				);
				
				_instances.push(instance);
			}
			
			return instance;
		}
		
		private function findInstance(sceneBindings	: DataBindings,
									  meshBindings 	: DataBindings) : EffectInstance
		{
			var numPasses : int = _instances.length;
			
			for (var passId : uint = 0; passId < numPasses; ++passId)
				if (_instances[passId].signature.isValid(sceneBindings, meshBindings))
					return _instances[passId];
			
			return null;
		}
		
		protected function initializePasses(sceneBindings	: DataBindingsProxy,
											meshBindings 	: DataBindingsProxy) : Vector.<Shader>
		{
			return new <Shader>[];
		}
	}
}
