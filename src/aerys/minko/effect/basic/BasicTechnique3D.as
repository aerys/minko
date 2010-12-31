package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3DTechnique;
	
	public class BasicTechnique3D extends AbstractEffect3DTechnique
	{
		public function BasicTechnique3D()
		{
			super();
			
			passes.push(new DiffusePass3D());
		}
		
		override public function get name() : String
		{
			return "basic";
		}
	}
}