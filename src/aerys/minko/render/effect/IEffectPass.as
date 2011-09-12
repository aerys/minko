package aerys.minko.render.effect
{
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	
	import flash.utils.Dictionary;

	public interface IEffectPass
	{
		function fillRenderState(state		: RendererState,
								 styleStack	: StyleStack, 
								 local		: TransformData, 
								 world		: Dictionary) : Boolean;
	}

}
