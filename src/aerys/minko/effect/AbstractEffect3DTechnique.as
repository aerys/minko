package aerys.minko.effect
{
	public class AbstractEffect3DTechnique implements IEffect3DTechnique
	{
		private var _passes		: Vector.<IEffect3DPass>	= new Vector.<IEffect3DPass>();
		private var _name		: String					= null;
		
		public function AbstractEffect3DTechnique(name : String)
		{
			_name = name;
		}
		
		public function get name() : String
		{
			return _name;
		}
		
		public function get passes() : Vector.<IEffect3DPass>
		{
			return _passes;
		}
	}
}