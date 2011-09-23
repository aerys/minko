package aerys.minko.render.effect
{
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	
	import flash.utils.Dictionary;
	

	public interface IEffect
	{
		function getPasses(styleData		: StyleStack, 
						   transformData	: TransformData, 
						   worldData		: Dictionary) : Vector.<IEffectPass>;
	}
}
