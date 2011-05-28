package aerys.minko.render.effect
{
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	
	import flash.utils.Dictionary;
	

	public interface IEffect
	{
		function getPasses(styleStack	: StyleStack, 
						   local		: LocalData, 
						   world		: Dictionary) : Vector.<IEffectPass>;
	}
}
