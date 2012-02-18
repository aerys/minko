package aerys.minko.scene.controller
{
	import aerys.minko.type.Signal;

	public interface IControllerTarget
	{
		function get controller() : AbstractController;
		function get controllerChanged() : Signal;
	}
}