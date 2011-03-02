package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect3D;
	import aerys.minko.effect.IEffect3DStyle;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	
	import flash.display3D.Context3DProgramType;
	
	public class BasicEffect3D extends AbstractEffect3D
	{
		public function BasicEffect3D()
		{
			super();
			
			techniques.push(new BasicTechnique3D());
			currentTechnique = techniques[0];
		}
		
		override public function begin(renderer : IRenderer3D, style : IEffect3DStyle) : void
		{
			super.begin(renderer, style);
			
			renderer.state.triangleCulling = style.get(BasicStyle3D.TRIANGLE_CULLING,
													   TriangleCulling.BACK);
		}
	}
}