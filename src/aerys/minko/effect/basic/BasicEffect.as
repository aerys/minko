package aerys.minko.effect.basic
{
	import aerys.minko.effect.AbstractEffect;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.state.TriangleCulling;
	
	public class BasicEffect extends AbstractEffect
	{
		public function BasicEffect()
		{
			super();
			
			passes[0] = new DiffusePass();
		}
		
	}
}
