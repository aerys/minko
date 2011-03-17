package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public class AbstractEffect3D implements IEffect3D
	{
		private var _techniques	: Vector.<IEffect3DTechnique>	= null;
		private var _current	: IEffect3DTechnique			= null;
		private var _style		: Effect3DStyle					= new Effect3DStyle();
		
		public function get style() : Effect3DStyle	{ return _style; }
		
		protected function get techniques() : Vector.<IEffect3DTechnique>	{ return _techniques; }
		
		public function AbstractEffect3D(...techniques)
		{
			_techniques = Vector.<IEffect3DTechnique>(techniques)
						  || new Vector.<IEffect3DTechnique>();
			
			if (_techniques.length)
				_current = _techniques[0];
		}
		
		public function getTechnique(name : String) : IEffect3DTechnique
		{
			var numTechniques : int = _techniques.length - 1;
			
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
		
		public function begin(renderer : IRenderer3D, style : Effect3DStyleStack) : void
		{
			// NOTHING
		}
		
		public function end(renderer : IRenderer3D, style : Effect3DStyleStack) : void
		{
			// NOTHING
		}
	}
}