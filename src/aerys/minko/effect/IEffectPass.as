package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;

	public interface IEffectPass
	{
		function fillRenderState(state	: RenderState,
								 style	: StyleStack, 
								 local	: TransformData, 
								 world	: Dictionary) : Boolean;
	}

}
