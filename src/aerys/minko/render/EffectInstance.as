package aerys.minko.render
{
	import aerys.minko.render.shader.Shader;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.Signature;

	public final class EffectInstance
	{
		private var _generator 		: Effect;
		private var _passes			: Vector.<Shader>;
		private var _extraPasses 	: Vector.<Shader>;
		private var _signature		: Signature;
		
		private var _passesChanged	: Signal;
		
		public function get effect() : Effect
		{
			return _generator;
		}
		
		public function get signature() : Signature
		{
			return _signature;
		}
		
		public function get passesChanged() : Signal
		{
			return _passesChanged;
		}
		
		public function get numPasses() : uint
		{
			return _passes.length + _extraPasses.length;
		}
		
		public function EffectInstance(generator 	: Effect,
									   passes		: Vector.<Shader>,
									   extraPasses 	: Vector.<Shader>,
									   signature	: Signature)
		{
			_generator = generator;
			_passes = passes;
			_extraPasses = extraPasses;
			_signature = signature;
			
			_passesChanged = new Signal('EffectInstance.passesChanged');
		}
		
		public function getPass(index : uint) : Shader
		{
			return index < _passes.length ? _passes[index] : _extraPasses[index];
		}
	}
}