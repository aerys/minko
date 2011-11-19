package aerys.minko.render.effect
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.shader.IShader;
	
	public class SinglePassRenderingEffect extends SinglePassEffect implements IRenderingEffect
	{
		public function SinglePassRenderingEffect(shader:IShader, priority:Number=0., renderTarget:RenderTarget=null)
		{
			super(shader, priority, renderTarget);
		}
	}
}