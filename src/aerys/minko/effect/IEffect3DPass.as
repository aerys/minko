package aerys.minko.effect
{
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.render.state.RenderState;
	
	import flash.utils.Dictionary;

	public interface IEffect3DPass
	{
		
		function fillRenderState(state	: RenderState,
								 style	: StyleStack3D, 
								 local	: TransformData, 
								 world	: Dictionary) : Boolean;
		
	}
}
