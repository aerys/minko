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
		
        private var _numUses        : uint;
        
		private var _passesChanged	: Signal;
        private var _retained       : Signal;
        private var _released       : Signal;
		
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
        
        public function get isDisposable() : Boolean
        {
            return _numUses == 0;
        }
        
//        public function 
		
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
            _retained = new Signal('EffectInstance.retained');
            _released = new Signal('EffectInstance.released');
		}
		
		public function getPass(index : uint) : Shader
		{
			var numPasses : uint = _passes.length;
			
			return index < numPasses ? _passes[index] : _extraPasses[uint(index - numPasses)];
		}
        
        public function retain() : void
        {
            ++_numUses;
            _retained.execute(this, _numUses);
        }
        
        public function release() : void
        {
            --_numUses;
            _released.execute(this, _numUses);
        }

	}
}