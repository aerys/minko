package aerys.minko.effect
{
	public class AbstractEffect3DTechnique implements IEffect3DTechnique
	{
		private var _passes		: Vector.<IEffect3DPass>	= new Vector.<IEffect3DPass>();
		
		public function AbstractEffect3DTechnique()
		{
			
		}
		
		public function get name() : String
		{
			return "AbstractEffect3DTechnique";
		}
		
		public function get passes() : Vector.<IEffect3DPass>
		{
			return _passes;
		}
	}
}