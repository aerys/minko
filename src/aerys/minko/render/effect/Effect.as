package aerys.minko.render.effect
{
	import aerys.minko.render.shader.ShaderTemplate;
	import aerys.minko.type.Signal;
	
	import flash.display.Shader;

	public class Effect
	{
		private var _passes		: Vector.<ShaderTemplate>	= null;
		private var _changed	: Signal						= new Signal();
		
		public function get changed() : Signal
		{
			return _changed;
		}
		
		public function get passes() : Vector.<ShaderTemplate>
		{
			return _passes;
		}
		
		public function Effect(...passes)
		{
			initialize(passes);
		}
		
		private function initialize(passes : Array) : void
		{
			while (passes[0] is Array)
				passes = passes[0];
			
			_passes = Vector.<ShaderTemplate>(passes);
		}
	}
}
