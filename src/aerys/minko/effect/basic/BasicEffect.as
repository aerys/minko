package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect;
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.effect.StyleStack;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.state.TriangleCulling;
	
	public class BasicEffect extends AbstractEffect
	{
		public function BasicEffect()
		{
			super();
			
			passes[0] = new DiffusePass();
		}
		
		override public function begin(renderer : IRenderer, style : StyleStack) : void
		{
			super.begin(renderer, style);
			
			renderer.state.triangleCulling = style.get(BasicStyle.TRIANGLE_CULLING,
													   TriangleCulling.BACK) as uint;
		}
	}
}
