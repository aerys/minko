package aerys.minko.render.effect
{
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.shader.PassTemplate;
	import aerys.minko.type.Signal;
	import aerys.minko.type.data.DataBindings;

	/**
	 * The base class to define effects.
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class Effect
	{
		use namespace minko_render;
		
		minko_render var _passes	: Vector.<PassTemplate>	= null;
		
		private var _passesChanged	: Signal				= new Signal('Effect.passesChanged');
		
		public function get passesChanged() : Signal
		{
			return _passesChanged;
		}
		
		public function get numPasses() : uint
		{
			return _passes.length;
		}
		
		public function Effect(...passes)
		{
			initialize(passes);
		}
		
		private function initialize(passes : Array) : void
		{
			while (passes[0] is Array)
				passes = passes[0];
			
			_passes = Vector.<PassTemplate>(passes);
		}
		
		public function getPass(index : uint = 0) : PassTemplate
		{
			return _passes[index];
		}
	}
}
