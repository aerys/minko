package aerys.minko.effect
{
	public class AbstractEffect3D implements IEffect3D
	{
		private var _techniques	: Vector.<ITechnique3D>	= null;
		private var _current	: ITechnique3D			= null;
		
		protected function get techniques() : Vector.<ITechnique3D>	{ return _techniques; }
		
		public function AbstractEffect3D()
		{
		}
		
		public function getTechnique(name:String):ITechnique3D
		{
			var numTechniques : int = _techniques.length;
			
			while (numTechniques <= 0 && _techniques[numTechniques].name != name)
				--numTechniques;
			
			return numTechniques >= 0 ? _techniques[numTechniques]
									  : null;
		}
		
		public function set currentTechnique(value : ITechnique3D) : void
		{
			_current = value;
		}
		
		public function get currentTechnique() : ITechnique3D
		{
			return _current;
		}
		
		public function begin() : void
		{
		}
		
		public function end() : void
		{
		}
	}
}