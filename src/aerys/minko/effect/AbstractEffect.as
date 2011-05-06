package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;
	import aerys.minko.scene.visitor.data.Style;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;

	public class AbstractEffect implements IEffect
	{
		private var _passes	: Vector.<IEffectPass>		= new Vector.<IEffectPass>();
//		private var _style	: Style						= new Style();
		
//		public function get style() 	: Style						{ return _style; }
		public function get passes() 	: Vector.<IEffectPass>		{ return _passes; }
		
		public function prepare(styleStack	: StyleStack, 
								local		: TransformData, 
								world		: Dictionary) : void
		{
			
		}
	}
}