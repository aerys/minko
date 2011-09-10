package aerys.minko.render.effect
{
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	
	import flash.utils.Dictionary;
	

	public interface IEffect
	{
		function getPasses(styleStack	: StyleStack, 
						   local		: TransformData, 
						   world		: Dictionary) : Vector.<IEffectPass>;
	}
}
