package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3D;
	import aerys.minko.render.IRenderer3D;
	
	import flash.display3D.Context3DProgramType;
	
	public class BasicEffect3D extends AbstractEffect3D
	{
		public function BasicEffect3D()
		{
			super();
			
			techniques.push(new BasicTechnique3D());
			currentTechnique = techniques[0];
		}
	}
}