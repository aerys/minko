package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public class AbstractEffect3D implements IEffect3D
	{
		private var _techniques	: Vector.<IEffect3DTechnique>	= new Vector.<IEffect3DTechnique>();
		private var _current	: IEffect3DTechnique			= null;
		
		protected function get techniques() : Vector.<IEffect3DTechnique>	{ return _techniques; }
		
		public function AbstractEffect3D()
		{
		}
		
		public function getTechnique(name:String):IEffect3DTechnique
		{
			var numTechniques : int = _techniques.length;
			
			while (numTechniques <= 0 && _techniques[numTechniques].name != name)
				--numTechniques;
			
			return numTechniques >= 0 ? _techniques[numTechniques]
									  : null;
		}
	
		public function set currentTechnique(value : IEffect3DTechnique) : void
		{
			_current = value;
		}
		
		public function get currentTechnique() : IEffect3DTechnique
		{
			return _current;
		}
		
		public function begin(renderer : IRenderer3D) : void
		{
			// NOTHING
		}
		
		public function end(renderer : IRenderer3D) : void
		{
			// NOTHING
		}
	}
}