package aerys.minko.render.effect
{
	import aerys.minko.render.renderer.state.RenderState;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	
	import flash.utils.Dictionary;

	public interface IEffectPass
	{
		function fillRenderState(state		: RenderState,
								 styleStack	: StyleStack, 
								 local		: LocalData, 
								 world		: Dictionary) : Boolean;
	}

}
