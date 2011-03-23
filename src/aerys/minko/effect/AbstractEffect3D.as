package aerys.minko.effect
{
	import aerys.minko.render.IRenderer3D;

	public class AbstractEffect3D implements IEffect3D
	{
		private var _passes	: Vector.<IEffect3DPass>	= new Vector.<IEffect3DPass>();
		private var _style	: Style3D					= new Style3D();
		
		public function get style() 	: Style3D					{ return _style; }
		public function get passes() 	: Vector.<IEffect3DPass>	{ return _passes; }
		
		public function AbstractEffect3D()
		{
		}
		
		public function begin(renderer : IRenderer3D, style : StyleStack3D) : void
		{
			// NOTHING
		}
		
		public function end(renderer : IRenderer3D, style : StyleStack3D) : void
		{
			// NOTHING
		}
	}
}