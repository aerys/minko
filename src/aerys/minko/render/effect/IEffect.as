package aerys.minko.render.effect
{
	import aerys.minko.scene.data.StyleData;
	import aerys.minko.scene.data.TransformData;
	
	import flash.utils.Dictionary;
	

	public interface IEffect
	{
		function getPasses(styleData		: StyleData, 
						   transformData	: TransformData, 
						   worldData		: Dictionary) : Vector.<IEffectPass>;
	}
}
