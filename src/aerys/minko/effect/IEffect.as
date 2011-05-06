package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;

	public interface IEffect
	{
		function get passes() : Vector.<IEffectPass>;
		
		function prepare(styleStack	: StyleStack, 
						 local		: TransformData, 
						 world		: Dictionary) : void;
	}
}
