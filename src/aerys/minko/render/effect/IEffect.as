package aerys.minko.render.effect
{
	import aerys.minko.render.renderer.IRenderer;
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
