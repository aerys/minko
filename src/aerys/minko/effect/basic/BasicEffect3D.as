package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3D;
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.effect.Style3D;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.state.TriangleCulling;
	
	public class BasicEffect3D extends AbstractEffect3D
	{
		public function BasicEffect3D()
		{
			super();
			
			passes[0] = new DiffusePass3D();
		}
		
		override public function begin(renderer : IRenderer3D, style : Style3D) : void
		{
			super.begin(renderer, style);
			
			renderer.state.triangleCulling = style.get(BasicStyle3D.TRIANGLE_CULLING,
													   TriangleCulling.BACK);
		}
	}
}