package aerys.minko.scene.node
{
	import aerys.minko.render.effect.Style;

	public interface IStylableScene extends IScene
	{
		function get style() : Style;
		function get styleEnabled() : Boolean;
	}
}