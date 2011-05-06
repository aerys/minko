package aerys.minko.effect
{
	import aerys.minko.query.renderdata.style.Style3D;
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.state.RenderState;
	
	import flash.utils.Dictionary;

	public class AbstractEffect3D implements IEffect3D
	{
		private var _passes	: Vector.<IEffect3DPass>	= new Vector.<IEffect3DPass>();
		private var _style	: Style3D					= new Style3D();
		
		public function get style() 	: Style3D					{ return _style; }
		public function get passes() 	: Vector.<IEffect3DPass>	{ return _passes; }
		
		public function AbstractEffect3D()
		{
		}
		
		public function prepare(styleStack	: StyleStack3D, 
								local		: TransformData, 
								world		: Dictionary) : void
		{
			
		}
		
	}
}