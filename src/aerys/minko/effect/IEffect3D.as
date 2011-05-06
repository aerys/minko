package aerys.minko.effect
{
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.interfaces.IStyled3D;
	
	import flash.utils.Dictionary;

	public interface IEffect3D extends IStyled3D
	{
		function get passes() : Vector.<IEffect3DPass>;
		
		function prepare(styleStack	: StyleStack3D, 
						 local		: TransformData, 
						 world		: Dictionary) : void;
		
	}
}
