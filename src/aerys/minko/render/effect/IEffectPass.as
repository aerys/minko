package aerys.minko.render.effect
{
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleData;

	import flash.utils.Dictionary;

	public interface IEffectPass
	{
		function fillRenderState(state			: RendererState,
								 styleData		: StyleData,
								 transformData	: TransformData,
								 worldData		: Dictionary) : Boolean;
	}
}
