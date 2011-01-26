package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3DTechnique;
	
	public class BasicTechnique3D extends AbstractEffect3DTechnique
	{
		public function BasicTechnique3D()
		{
			super("basic");
			
			passes.push(new DiffusePass3D());
		}
	}
}