package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3D;
	import aerys.minko.effect.Effect3DStyleStack;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.state.TriangleCulling;
	
	public class BasicEffect3D extends AbstractEffect3D
	{
		public function BasicEffect3D()
		{
			super();
			
			techniques.push(new BasicTechnique3D());
			currentTechnique = techniques[0];
		}
		
		override public function begin(renderer : IRenderer3D, style : Effect3DStyleStack) : void
		{
			super.begin(renderer, style);
			
			renderer.state.triangleCulling = style.get(BasicStyle3D.TRIANGLE_CULLING,
													   TriangleCulling.BACK);
		}
	}
}
