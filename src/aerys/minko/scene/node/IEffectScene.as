package aerys.minko.scene.node
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IRenderingEffect;

	public interface IEffectScene
	{
		function get effect() : IRenderingEffect;
	}
}