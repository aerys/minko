package aerys.minko.effect
{
	import aerys.minko.render.IRenderer;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
	import flash.utils.Dictionary;

	public interface IEffect
	{
		function getPasses(styleStack	: StyleStack, 
						   local		: TransformData, 
						   world		: Dictionary) : Vector.<IEffectPass>;
	}
}
