package aerys.minko.render.effect
{
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.ShaderSignature;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;

	public final class EffectInstance
	{
		private var _effect		: Effect					= null;
		private var _target		: Mesh						= null;
		
		private var _passes		: Vector.<ShaderInstance>	= new <ShaderInstance>[];
		private var _drawCalls	: Vector.<DrawCall>			= new <DrawCall>[];
		
		public function get passes() : Vector.<ShaderInstance>
		{
			return _passes;
		}
		
		public function get drawCalls() : Vector.<DrawCall>
		{
			return _drawCalls;
		}
		
		public function EffectInstance(effect : Effect, target : Mesh)
		{
			_effect = effect;
			_target = target;
			
			initialize();
		}
		
		private function initialize() : void
		{
			var numPasses 		: int			= _effect.numPasses;
			var localBindings	: DataBindings	= _target.localBindings;
			var globalBindings	: DataBindings	= (_target.root as Scene).globalBindings;
			
			for (var passId : int = 0; passId < numPasses; ++passId)
			{
				var instance 		: ShaderInstance 	= _effect.getPass(passId).instanciate(
					localBindings,
					globalBindings
				);
				var drawCall		: DrawCall			= instance.program.createDrawCall();
				
				_target.initializeDrawCall(drawCall);
				
				_passes[passId] = instance;
				_drawCalls[passId] = drawCall;
			}
		}
		
		public function dispose() : void
		{
			trace("EffectInstance.dispose()");
		}
	}
}