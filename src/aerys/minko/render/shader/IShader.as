package aerys.minko.render.shader
{
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	
	import flash.utils.Dictionary;

	public interface IShader
	{
		function get name() : String;
		
		function fillRenderState(state			: RendererState,
								 styleData		: StyleData,
								 transformData	: TransformData,
								 worldData		: Dictionary) : void;
	}
}
