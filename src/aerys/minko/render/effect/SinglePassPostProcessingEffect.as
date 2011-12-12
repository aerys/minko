package aerys.minko.render.effect
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.shader.IShader;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.ViewportData;
	
	import flash.utils.Dictionary;
	
	public class SinglePassPostProcessingEffect extends SinglePassEffect implements IPostProcessingEffect
	{
		public function SinglePassPostProcessingEffect(shader : IShader)
		{
			super(shader, -1.);
		}
		
		override public function fillRenderState(state			: RendererState,
												 styleData		: StyleData,
												 transformData	: TransformData,
												 worldData		: Dictionary) : Boolean
		{
			super.fillRenderState(state, styleData, transformData, worldData);
			
			state.renderTarget = (worldData[ViewportData] as ViewportData).backBufferRenderTarget;
			
			return true;
		}
	}
}